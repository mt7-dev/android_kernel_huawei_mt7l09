/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_bbp_int.h */
/* Version       : 2.0 */
/* Author        : xxx*/
/* Created       : 2013-02-22*/
/* Last Modified : */
/* Description   :  The C union definition file for the module bbp_int*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Author        : xxx*/
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_BBP_INT_H__
#define __HI_BBP_INT_H__

/*
 * Project: hi
 * Module : bbp_int
 */

#ifndef HI_SET_GET
#define HI_SET_GET(a0,a1,a2,a3,a4)
#endif

/********************************************************************************/
/*    bbp_int 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET                 (0x0) /* 寄存器。 */
#define    HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET                 (0x4) /* 寄存器。 */
#define    HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET                 (0x8) /* 寄存器。 */
#define    HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET                 (0xC) /* 寄存器。 */
#define    HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET                 (0x10) /* 寄存器。 */
#define    HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET                 (0x14) /* 寄存器。 */
#define    HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET                 (0x18) /* 寄存器。 */
#define    HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET                 (0x1C) /* 寄存器。 */
#define    HI_BBP_INT_WARN_INT_MSK_OFFSET                    (0x20) /* 寄存器。 */
#define    HI_BBP_INT_WARN_INT_CLS_OFFSET                    (0x24) /* 寄存器。 */
#define    HI_BBP_INT_WARN_INT_TYP_OFFSET                    (0x28) /* 寄存器。 */
#define    HI_BBP_INT_ARM_INT_MSK_OFFSET                     (0x30) /* 寄存器。 */
#define    HI_BBP_INT_ARM_INT_CLS_OFFSET                     (0x34) /* 寄存器。 */
#define    HI_BBP_INT_ARM_INT_TYP_OFFSET                     (0x38) /* 寄存器。 */
#define    HI_BBP_INT_ARM_INT_ALM_OFFSET                     (0x3C) /* 寄存器。 */
#define    HI_BBP_INT_APP_ARM_INT_MSK_OFFSET                 (0x40) /* 寄存器。 */
#define    HI_BBP_INT_APP_ARM_INT_CLS_OFFSET                 (0x44) /* 寄存器。 */
#define    HI_BBP_INT_APP_ARM_INT_TYP_OFFSET                 (0x48) /* 寄存器。 */
#define    HI_BBP_INT_PRIOR_LEVEL_OFFSET                     (0x50) /* 寄存器。 */
#define    HI_BBP_INT_INT2OUT_BAK1_OFFSET                    (0x60) /* 寄存器。 */
#define    HI_BBP_INT_INT2OUT_BAK2_OFFSET                    (0x64) /* 寄存器。 */
#define    HI_BBP_INT_INT2OUT_BAK3_OFFSET                    (0x68) /* 寄存器。 */
#define    HI_BBP_INT_INT2OUT_BAK4_OFFSET                    (0x6C) /* 路测中断状态寄存器。 */
#define    HI_BBP_INT_INT2OUT_BAK5_OFFSET                    (0x70) /* 路测中断屏蔽寄存器。 */
#define    HI_BBP_INT_INT2OUT_BAK6_OFFSET                    (0x74) /* 寄存器。 */
#define    HI_BBP_INT_INT2OUT_BAK7_OFFSET                    (0x78) /* 寄存器。 */
#define    HI_BBP_INT_INT2OUT_BAK8_OFFSET                    (0x7C) /* 寄存器。 */
#define    HI_BBP_INT_CLK_EN_SEL_OFFSET                      (0x160) /* 寄存器。 */
#define    HI_BBP_INT_AHB2LBUS_CLK_SEL_OFFSET                (0x164) /* 寄存器。 */
#define    HI_BBP_INT_RFIN_SRST_EN_OFFSET                    (0x178) /* 寄存器。 */
#define    HI_BBP_INT_FPU_SRST_EN_OFFSET                     (0x17C) /* 寄存器。 */
#define    HI_BBP_INT_UL_SRST_EN_OFFSET                      (0x180) /* 寄存器。 */
#define    HI_BBP_INT_VDL_SRST_EN_OFFSET                     (0x184) /* 寄存器。 */
#define    HI_BBP_INT_PB_SRST_EN_OFFSET                      (0x188) /* 寄存器。 */
#define    HI_BBP_INT_DBG_SRST_EN_OFFSET                     (0x18C) /* 寄存器。 */
#define    HI_BBP_INT_AXIM_SRST_EN_OFFSET                    (0x190) /* 寄存器。 */
#define    HI_BBP_INT_AXIS_SRST_EN_OFFSET                    (0x194) /* 寄存器。 */
#define    HI_BBP_INT_AHBS_SRST_EN_OFFSET                    (0x198) /* 寄存器。 */
#define    HI_BBP_INT_INT_SRST_EN_OFFSET                     (0x1A4) /* 寄存器。 */
#define    HI_BBP_INT_STU_SRST_EN_OFFSET                     (0x1A8) /* 寄存器。 */
#define    HI_BBP_INT_PDM_SRST_EN_OFFSET                     (0x1B4) /* 寄存器。 */
#define    HI_BBP_INT_ATPRAM_CTRL_OFFSET                     (0x200) /* 寄存器。 */


#ifndef __ASSEMBLY__

/********************************************************************************/
/*    bbp_int 寄存器定义（项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    dsp_sym_int_msk_0          : 1; /* [0..0] 0.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_1          : 1; /* [1..1] 1.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_2          : 1; /* [2..2] 2.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_3          : 1; /* [3..3] 3.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_4          : 1; /* [4..4] 4.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_5          : 1; /* [5..5] 5.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_6          : 1; /* [6..6] 6.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_7          : 1; /* [7..7] 7.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_8          : 1; /* [8..8] 8.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_9          : 1; /* [9..9] 9.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_10         : 1; /* [10..10] 10.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_mask_11        : 1; /* [11..11] 11.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_12         : 1; /* [12..12] 12.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dsp_sym_int_msk_13         : 1; /* [13..13] 13.5符号中断屏蔽。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    Reserved                   : 18; /* [31..14] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_DSP_SYM_INT_MSK_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    dsp_sym_int_cls_0          : 1; /* [0..0] 0.5符号中断清除寄存器。向此寄存器位写1，清除0.5符号中断 */
        unsigned int    dsp_sym_int_cls_1          : 1; /* [1..1] 1.5符号中断清除寄存器。向此寄存器位写1，清除1.5符号中断 */
        unsigned int    dsp_sym_int_cls_2          : 1; /* [2..2] 2.5符号中断清除寄存器。向此寄存器位写1，清除2.5符号中断 */
        unsigned int    dsp_sym_int_cls_3          : 1; /* [3..3] 3.5符号中断清除寄存器。向此寄存器位写1，清除3.5符号中断 */
        unsigned int    dsp_sym_int_cls_4          : 1; /* [4..4] 4.5符号中断清除寄存器。向此寄存器位写1，清除4.5符号中断 */
        unsigned int    dsp_sym_int_cls_5          : 1; /* [5..5] 5.5符号中断清除寄存器。向此寄存器位写1，清除5.5符号中断 */
        unsigned int    dsp_sym_int_cls_6          : 1; /* [6..6] 6.5符号中断清除寄存器。向此寄存器位写1，清除6.5符号中断 */
        unsigned int    dsp_sym_int_cls_7          : 1; /* [7..7] 7.5符号中断清除寄存器。向此寄存器位写1，清除7.5符号中断 */
        unsigned int    dsp_sym_int_cls_8          : 1; /* [8..8] 8.5符号中断清除寄存器。向此寄存器位写1，清除8.5符号中断 */
        unsigned int    dsp_sym_int_cls_9          : 1; /* [9..9] 9.5符号中断清除寄存器。向此寄存器位写1，清除9.5符号中断 */
        unsigned int    dsp_sym_int_cls_10         : 1; /* [10..10] 10.5符号中断清除寄存器。向此寄存器位写1，清除10.5符号中断 */
        unsigned int    dsp_sym_int_cls_11         : 1; /* [11..11] 11.5符号中断清除寄存器。向此寄存器位写1，清除11.5符号中断 */
        unsigned int    dsp_sym_int_cls_12         : 1; /* [12..12] 12.5符号中断清除寄存器。向此寄存器位写1，清除12.5符号中断 */
        unsigned int    dsp_sym_int_cls_13         : 1; /* [13..13] 13.5符号中断清除寄存器。向此寄存器位写1，清除13.5符号中断 */
        unsigned int    Reserved                   : 18; /* [31..14] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_DSP_SYM_INT_CLS_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    dsp_sym_int_typ_0          : 1; /* [0..0] 0.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_1          : 1; /* [1..1] 1.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_2          : 1; /* [2..2] 2.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_3          : 1; /* [3..3] 3.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_4          : 1; /* [4..4] 4.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_5          : 1; /* [5..5] 5.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_6          : 1; /* [6..6] 6.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_7          : 1; /* [7..7] 7.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_8          : 1; /* [8..8] 8.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_9          : 1; /* [9..9] 9.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_10         : 1; /* [10..10] 10.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_11         : 1; /* [11..11] 11.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_12         : 1; /* [12..12] 12.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    dsp_sym_int_typ_13         : 1; /* [13..13] 13.5符号中断标志指示。向清除寄存器的对应位写1清零中断标志 */
        unsigned int    Reserved                   : 18; /* [31..14] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_DSP_SYM_INT_TYP_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    dsp_sym_int_alm_0          : 1; /* [0..0] 0.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_1          : 1; /* [1..1] 1.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_2          : 1; /* [2..2] 2.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_3          : 1; /* [3..3] 3.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_4          : 1; /* [4..4] 4.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_5          : 1; /* [5..5] 5.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_6          : 1; /* [6..6] 6.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_7          : 1; /* [7..7] 7.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_8          : 1; /* [8..8] 8.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_9          : 1; /* [9..9] 9.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_10         : 1; /* [10..10] 10.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_11         : 1; /* [11..11] 11.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_12         : 1; /* [12..12] 12.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_sym_int_alm_13         : 1; /* [13..13] 13.5符号中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    Reserved                   : 18; /* [31..14] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_DSP_SYM_INT_ALM_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    dsp_cells_int_msk          : 1; /* [0..0] 小区搜索中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_pwrm_int_msk           : 1; /* [1..1] 能量测量中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_cqi_int_msk            : 1; /* [2..2] cqi中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_pb_int_msk             : 1; /* [3..3] pb中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_pdu0_int_msk           : 1; /* [4..4] 信道估计中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_pdu1_int_msk           : 1; /* [5..5] pds解调中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_pdu2_int_msk           : 1; /* [6..6] pdc解调中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_cmu_int_msk            : 1; /* [7..7] cmu中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_vdl_int_msk            : 1; /* [8..8] vdl中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_cfi_int_msk            : 1; /* [9..9] cfi中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_dma_int_msk            : 1; /* [10..10] dma中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_tdl_int_mask           : 1; /* [11..11] tdl中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_dcf_int_msk            : 1; /* [12..12] dcf中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_ul_int_msk             : 1; /* [13..13] ul中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_fpu_int_msk            : 1; /* [14..14] fpu中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_aagc_int_msk           : 1; /* [15..15] aagc中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_rstd_int_msk           : 1; /* [16..16] rstd中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_dma_anten0_int_msk     : 1; /* [17..17] dma_anten_0中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_dma_anten1_int_msk     : 1; /* [18..18] dma_anten_1中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_wlan_priority_int_msk  : 1; /* [19..19] wlan_bt_priority中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    dsp_wlan_tx_active_int_msk : 1; /* [20..20] wlan_bt_tx_active中断屏蔽。1表示打开中断上报；0表示关闭中断上报 */
        unsigned int    Reserved                   : 11; /* [31..21] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_DSP_MOD_INT_MSK_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    dsp_cells_int_cls          : 1; /* [0..0] 小区搜索中断清除寄存器。向此寄存器位写1，清除小区搜索中断 */
        unsigned int    dsp_pwrm_int_cls           : 1; /* [1..1] 能量测量中断清除寄存器。向此寄存器位写1，清除能量测量中断 */
        unsigned int    dsp_cqi_int_cls            : 1; /* [2..2] cqi中断清除寄存器。向此寄存器位写1，清除cqi中断 */
        unsigned int    dsp_pb_int_cls             : 1; /* [3..3] pb中断清除寄存器。向此寄存器位写1，清除pb中断 */
        unsigned int    dsp_pdu0_int_cls           : 1; /* [4..4] 信道估计中断清除寄存器。向此寄存器位写1，清除信道估计中断 */
        unsigned int    dsp_pdu1_int_cls           : 1; /* [5..5] pds解调中断清除寄存器。向此寄存器位写1，清除pds解调中断 */
        unsigned int    dsp_pdu2_int_cls           : 1; /* [6..6] pdc解调中断清除寄存器。向此寄存器位写1，清除pdc解调中断 */
        unsigned int    dsp_cmu_int_cls            : 1; /* [7..7] cmu中断清除寄存器。向此寄存器位写1，清除cmu中断 */
        unsigned int    dsp_vdl_int_cls            : 1; /* [8..8] vdl中断清除寄存器。向此寄存器位写1，清除vdl中断 */
        unsigned int    dsp_cfi_int_cls            : 1; /* [9..9] cfi中断清除寄存器。向此寄存器位写1，清除cfi中断 */
        unsigned int    dsp_dma_int_cls            : 1; /* [10..10] dma中断清除寄存器。向此寄存器位写1，清除dma中断 */
        unsigned int    dsp_tdl_int_cls            : 1; /* [11..11] tdl中断清除寄存器。向此寄存器位写1，清除tdl中断 */
        unsigned int    dsp_dcf_int_cls            : 1; /* [12..12] dcf中断清除寄存器。向此寄存器位写1，清除dcf中断 */
        unsigned int    dsp_ul_int_cls             : 1; /* [13..13] ul中断清除寄存器。向此寄存器位写1，清除ul中断 */
        unsigned int    dsp_fpu_int_cls            : 1; /* [14..14] fpu中断清除寄存器。向此寄存器位写1，清除fpu中断 */
        unsigned int    dsp_aagc_int_cls           : 1; /* [15..15] aagc中断清除寄存器。向此寄存器位写1，清除aagc中断 */
        unsigned int    dsp_rstd_int_cls           : 1; /* [16..16] rstd中断清除寄存器。向此寄存器位写1，清除rstd中断 */
        unsigned int    dsp_dma_anten0_int_cls     : 1; /* [17..17] dma_anten_0中断清除寄存器。向此寄存器位写1，清除dma_anten_0中断 */
        unsigned int    dsp_dma_anten1_int_cls     : 1; /* [18..18] dma_anten_1中断清除寄存器。向此寄存器位写1，清除dma_anten_1中断 */
        unsigned int    dsp_wlan_priority_int_cls  : 1; /* [19..19] wlan_bt_priority中断清除寄存器。向此寄存器位写1，清除wlan_bt_priority中断 */
        unsigned int    dsp_wlan_tx_active_int_cls : 1; /* [20..20] wlan_bt_tx_active中断清除寄存器。向此寄存器位写1，清除wlan_bt_tx_active中断 */
        unsigned int    Reserved                   : 11; /* [31..21] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_DSP_MOD_INT_CLS_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    dsp_cells_int_typ          : 1; /* [0..0] 小区搜索中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_pwrm_int_typ           : 1; /* [1..1] 能量测量中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_cqi_int_typ            : 1; /* [2..2] cqi中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_pb_int_typ             : 1; /* [3..3] pb中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_pdu0_int_typ           : 1; /* [4..4] 信道估计中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_pdu1_int_typ           : 1; /* [5..5] pds解调中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_pdu2_int_typ           : 1; /* [6..6] pdc解调中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_cmu_int_typ            : 1; /* [7..7] cmu中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_vdl_int_typ            : 1; /* [8..8] vdl中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_cfi_int_typ            : 1; /* [9..9] cfi中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_dma_int_typ            : 1; /* [10..10] dma中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_tdl_int_typ            : 1; /* [11..11] tdl中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_dcf_int_typ            : 1; /* [12..12] dcf中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_ul_int_typ             : 1; /* [13..13] ul中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_fpu_int_typ            : 1; /* [14..14] fpu中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_aagc_int_typ           : 1; /* [15..15] aagc中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_rstd_int_typ           : 1; /* [16..16] rstd中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_dma_anten0_int_typ     : 1; /* [17..17] dma_anten_0中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_dma_anten1_int_typ     : 1; /* [18..18] dma_anten_1中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_wlan_priority_int_typ  : 1; /* [19..19] wlan_bt_priority中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    dsp_wlan_tx_active_int_typ : 1; /* [20..20] wlan_bt_tx_active中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    Reserved                   : 11; /* [31..21] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_DSP_MOD_INT_TYP_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    dsp_cells_int_alm          : 1; /* [0..0] 小区搜索中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_pwrm_int_alm           : 1; /* [1..1] 能量测量中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_cqi_int_alm            : 1; /* [2..2] cqi中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_pb_int_alm             : 1; /* [3..3] pb中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_pdu0_int_alm           : 1; /* [4..4] 信道估计中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_pdu1_int_alm           : 1; /* [5..5] pds解调中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_pdu2_int_alm           : 1; /* [6..6] pdc解调中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_cmu_int_alm            : 1; /* [7..7] cmu中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_vdl_int_alm            : 1; /* [8..8] vdl中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_cfi_int_alm            : 1; /* [9..9] cfi中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_dma_int_alm            : 1; /* [10..10] dma中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_tdl_int_alm            : 1; /* [11..11] tdl中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_dcf_int_alm            : 1; /* [12..12] dcf中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_ul_int_alm             : 1; /* [13..13] ul中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_fpu_int_alm            : 1; /* [14..14] fpu中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_aagc_int_alm           : 1; /* [15..15] aagc中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_rstd_int_alm           : 1; /* [16..16] rstd中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_dma_anten0_int_alm     : 1; /* [17..17] dma_anten_0中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_dma_anten1_int_alm     : 1; /* [18..18] dma_anten_1中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_wlan_priority_int_alm  : 1; /* [19..19] wlan_bt_priority中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    dsp_wlan_tx_active_int_alm : 1; /* [20..20] wlan_bt_tx_active中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    Reserved                   : 11; /* [31..21] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_DSP_MOD_INT_ALM_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    rfin_warn_int_msk          : 1; /* [0..0] rfin告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    fpu_warn_int_msk           : 1; /* [1..1] fpu告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    ul_warn_int_msk            : 1; /* [2..2] ul告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    cqi_warn_int_msk           : 1; /* [3..3] cqi告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    pb_warn_int_msk            : 1; /* [4..4] pb告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    vdl_warn_int_msk           : 1; /* [5..5] vdl告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    pdu_warn_int_msk           : 1; /* [6..6] pdu告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    tdl_warn_int_msk           : 1; /* [7..7] tdl告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    dma_warn_int_msk           : 1; /* [8..8] dma告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    Reserved                   : 23; /* [31..9] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_WARN_INT_MSK_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    rfin_warn_int_cls          : 1; /* [0..0] rfin告警中断清除寄存器。向此寄存器位写1，清除ul中断 */
        unsigned int    fpu_warn_int_cls           : 1; /* [1..1] fpu告警中断清除寄存器。向此寄存器位写1，清除ul中断 */
        unsigned int    ul_warn_int_cls            : 1; /* [2..2] ul告警中断清除寄存器。向此寄存器位写1，清除ul中断 */
        unsigned int    cqi_warn_int_cls           : 1; /* [3..3] cqi告警中断清除寄存器。向此寄存器位写1，清除ul中断 */
        unsigned int    pb_warn_int_cls            : 1; /* [4..4] pb告警中断清除寄存器。向此寄存器位写1，清除ul中断 */
        unsigned int    vdl_warn_int_cls           : 1; /* [5..5] vdl告警中断清除寄存器。向此寄存器位写1，清除ul中断 */
        unsigned int    pdu_warn_int_cls           : 1; /* [6..6] pdu告警中断清除寄存器。向此寄存器位写1，清除ul中断 */
        unsigned int    tdl_warn_int_cls           : 1; /* [7..7] tdl告警中断清除寄存器。向此寄存器位写1，清除ul中断 */
        unsigned int    dma_warn_int_cls           : 1; /* [8..8] dma告警中断清除寄存器。向此寄存器位写1，清除ul中断 */
        unsigned int    Reserved                   : 23; /* [31..9] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_WARN_INT_CLS_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    rfin_warn_int_typ          : 1; /* [0..0] rfin告警中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    fpu_warn_int_typ           : 1; /* [1..1] fpu告警中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    ul_warn_int_typ            : 1; /* [2..2] ul告警中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    cqi_warn_int_typ           : 1; /* [3..3] cqi告警中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    pb_warn_int_typ            : 1; /* [4..4] pb告警中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    vdl_warn_int_typ           : 1; /* [5..5] vdl告警中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    pdu_warn_int_typ           : 1; /* [6..6] pdu告警中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    tdl_warn_int_typ           : 1; /* [7..7] tdl告警中断寄存器。指示中断状态。才 */
        unsigned int    dma_warn_int_typ           : 1; /* [8..8] dma告警中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    Reserved                   : 23; /* [31..9] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_WARN_INT_TYP_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    arm_position_int_msk       : 1; /* [0..0] arm子帧定时中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    arm_dl_dma_int_msk         : 1; /* [1..1] arm的dl_dma中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    Reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_ARM_INT_MSK_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    arm_position_int_cls       : 1; /* [0..0] arm子帧定时中断清除寄存器。向此寄存器位写1，清除子帧定时中断 */
        unsigned int    arm_dl_dma_int_cls         : 1; /* [1..1] arm的dl_dma中断清除寄存器。向此寄存器位写1，清除dl_dma中断 */
        unsigned int    Reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_ARM_INT_CLS_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    arm_position_int_typ       : 1; /* [0..0] arm子帧定时中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    arm_dl_dma_int_typ         : 1; /* [1..1] arm的dl_dma中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    Reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_ARM_INT_TYP_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    arm_position_int_alm       : 1; /* [0..0] arm子帧定时中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    arm_dl_dma_int_alm         : 1; /* [1..1] arm的dl_dma中断告警指示。当新中断触发而原中断没被处理清除时产生告警，写1清0告警位 */
        unsigned int    Reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_ARM_INT_ALM_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    aarm_dbg_msk               : 1; /* [0..0] app arm的dbg告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    aarm_dma_msk               : 1; /* [1..1] app arm的dma告警中断屏蔽寄存器。0：屏蔽中断上报；1：使能中断上报。初值为0 */
        unsigned int    Reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_APP_ARM_INT_MSK_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    aarm_dbg_cls               : 1; /* [0..0] app arm的dbg告警中断清除寄存器。向此寄存器位写1，清除dbg告警中断 */
        unsigned int    aarm_dma_cls               : 1; /* [1..1] app arm的dma告警中断清除寄存器。向此寄存器位写1，清除dbg告警中断 */
        unsigned int    Reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_APP_ARM_INT_CLS_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    aarm_dbg_typ               : 1; /* [0..0] app arm的dbg告警中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    aarm_dma_typ               : 1; /* [1..1] app arm的dma告警中断寄存器。指示中断状态。0：无中断；1：有上报中断。 */
        unsigned int    Reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_APP_ARM_INT_TYP_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    prior_level                : 2; /* [1..0] 总线优先级 */
        unsigned int    Reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_PRIOR_LEVEL_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    xs_rs_gated_en             : 1; /* [0..0] AXI Slave总线时钟自动门控寄存器1：自动门控功能打开0：自动门控功能关闭，不降低功耗 */
        unsigned int    hs_rs_gated_en             : 1; /* [1..1] AXI_Sync Slave总线时钟自动门控寄存器1：自动门控功能打开0：自动门控功能关闭，不降低功耗 */
        unsigned int    xm_gated_en                : 1; /* [2..2] AXI Master总线时钟自动门控寄存器1：自动门控功能打开0：自动门控功能关闭，不降低功耗 */
        unsigned int    xs_gs_gated_en             : 1; /* [3..3] AXI Slave总线时钟自动门控寄存器1：自动门控功能打开0：自动门控功能关闭，不降低功耗 */
        unsigned int    hs_gs_gated_en             : 1; /* [4..4] AXI_Sync Slave总线时钟自动门控寄存器1：自动门控功能打开0：自动门控功能关闭，不降低功耗 */
        unsigned int    int2out_bak1               : 11; /* [15..5] 测试寄存器 */
        unsigned int    Reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_INT2OUT_BAK1_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    int2out_bak2               : 16; /* [15..0] 测试寄存器 */
        unsigned int    Reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_INT2OUT_BAK2_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    int2out_bak3               : 16; /* [15..0] 测试寄存器 */
        unsigned int    Reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_INT2OUT_BAK3_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    dt0_int_typ                : 1; /* [0..0] 路测中断0状态寄存器。0：无中断；1：有上报中断。写1清0 */
        unsigned int    dt1_int_typ                : 1; /* [1..1] 路测中断1状态寄存器。0：无中断；1：有上报中断。写1清0 */
        unsigned int    dt2_int_typ                : 1; /* [2..2] 路测中断2状态寄存器。0：无中断；1：有上报中断。写1清0 */
        unsigned int    dt3_int_typ                : 1; /* [3..3] 路测中断3状态寄存器。0：无中断；1：有上报中断。写1清0 */
        unsigned int    dt4_int_typ                : 1; /* [4..4] 路测中断4状态寄存器。0：无中断；1：有上报中断。写1清0 */
        unsigned int    dt5_int_typ                : 1; /* [5..5] 路测中断5状态寄存器。0：无中断；1：有上报中断。写1清0 */
        unsigned int    dt6_int_typ                : 1; /* [6..6] 路测中断6状态寄存器。0：无中断；1：有上报中断。写1清0 */
        unsigned int    dt7_int_typ                : 1; /* [7..7] 路测中断7状态寄存器。0：无中断；1：有上报中断。写1清0 */
        unsigned int    dt0_int_alm                : 1; /* [8..8] 路测中断0告警指示。当新中断触发而原中断没被处理并清除时产生告警，写1清0告警位 */
        unsigned int    dt1_int_alm                : 1; /* [9..9] 路测中断1告警指示。当新中断触发而原中断没被处理并清除时产生告警，写1清0告警位 */
        unsigned int    dt2_int_alm                : 1; /* [10..10] 路测中断2告警指示。当新中断触发而原中断没被处理并清除时产生告警，写1清0告警位 */
        unsigned int    dt3_int_alm                : 1; /* [11..11] 路测中断3告警指示。当新中断触发而原中断没被处理并清除时产生告警，写1清0告警位 */
        unsigned int    dt4_int_alm                : 1; /* [12..12] 路测中断4告警指示。当新中断触发而原中断没被处理并清除时产生告警，写1清0告警位 */
        unsigned int    dt5_int_alm                : 1; /* [13..13] 路测中断5告警指示。当新中断触发而原中断没被处理并清除时产生告警，写1清0告警位 */
        unsigned int    dt6_int_alm                : 1; /* [14..14] 路测中断6告警指示。当新中断触发而原中断没被处理并清除时产生告警，写1清0告警位 */
        unsigned int    dt7_int_alm                : 1; /* [15..15] 路测中断7告警指示。当新中断触发而原中断没被处理并清除时产生告警，写1清0告警位 */
        unsigned int    Reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_INT2OUT_BAK4_T;    /* 路测中断状态寄存器。 */

typedef union
{
    struct
    {
        unsigned int    Reserved_1                 : 8; /* [7..0] 保留 */
        unsigned int    dt_int_msk                 : 1; /* [8..8] 路测中断屏蔽寄存器，屏蔽８个路测中断。0表示关闭中断上报；1表示打开中断上报； */
        unsigned int    Reserved_0                 : 23; /* [31..9] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_INT2OUT_BAK5_T;    /* 路测中断屏蔽寄存器。 */

typedef union
{
    struct
    {
        unsigned int    int2out_bak6               : 16; /* [15..0] 测试寄存器 */
        unsigned int    Reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_INT2OUT_BAK6_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    int2out_bak7               : 16; /* [15..0] 测试寄存器 */
        unsigned int    Reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_INT2OUT_BAK7_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    int2out_bak8               : 16; /* [15..0] 测试寄存器 */
        unsigned int    Reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_INT2OUT_BAK8_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    Reserved_4                 : 1; /* [0..0] 保留 */
        unsigned int    Reserved_3                 : 1; /* [1..1] 保留 */
        unsigned int    rfin_clk_sel               : 1; /* [2..2] rfin模块时钟(rfin_clk)使能选择。0：表示不使能模块时钟；1：表示使能模块时钟；初始值为0 */
        unsigned int    fpu_clk_sel                : 1; /* [3..3] fpu模块时钟(fpu_clk)使能选择。0：表示不使能模块时钟；1：表示使能模块时钟；初始值为0 */
        unsigned int    ul_clk_sel                 : 1; /* [4..4] ul模块时钟(ul_clk)使能选择。0：表示不使能模块时钟；1：表示使能模块时钟；初始值为0 */
        unsigned int    vdl_clk_sel                : 1; /* [5..5] vdl模块时钟(vdl_clk)使能选择。0：表示不使能模块时钟；1：表示使能模块时钟；初始值为0 */
        unsigned int    pb_clk_sel                 : 1; /* [6..6] pb模块时钟(pb_clk)使能选择。0：表示不使能模块时钟；1：表示使能模块时钟；初始值为0 */
        unsigned int    dbg_clk_sel                : 1; /* [7..7] dbg模块时钟(dbg_clk)使能选择。0：表示不使能模块时钟；1：表示使能模块时钟；初始值为0 */
        unsigned int    axim_clk_sel               : 1; /* [8..8] axim模块时钟(axim_clk)使能选择。0：表示不使能模块时钟；1：表示使能模块时钟；初始值为0 */
        unsigned int    axis_clk_sel               : 1; /* [9..9] axis模块时钟(axis_clk)使能选择。0：表示不使能模块时钟；1：表示使能模块时钟；初始值为0 */
        unsigned int    ahbs_clk_sel               : 1; /* [10..10] ahbs模块时钟(ahbs_clk)使能选择。0：表示不使能模块时钟；1：表示使能模块时钟；初始值为0 */
        unsigned int    Reserved_2                 : 1; /* [11..11] 保留 */
        unsigned int    Reserved_1                 : 1; /* [12..12] 保留 */
        unsigned int    pdm_clk_sel                : 1; /* [13..13] pdm模块时钟(pdm_clk)使能选择。0：表示不使能模块时钟；1：表示使能模块时钟；初始值为0 */
        unsigned int    Reserved_0                 : 18; /* [31..14] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_CLK_EN_SEL_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    ahb2lbus_clk_sel           : 1; /* [0..0] AHB2LBUS模块RAM时钟门控使能。置0表示时钟常开；置1表示时钟做自动门控；初始值为0 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_AHB2LBUS_CLK_SEL_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    rfin_srst_en               : 1; /* [0..0] rfin子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0，复位期间该模块寄存器无法访问 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_RFIN_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    fpu_srst_en                : 1; /* [0..0] fpu子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0，复位期间该模块寄存器无法访问 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_FPU_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    ul_srst_en                 : 1; /* [0..0] ul子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0，复位期间该模块寄存器无法访问 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_UL_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    vdl_srst_en                : 1; /* [0..0] vdl子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_VDL_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    pb_srst_en                 : 1; /* [0..0] pb子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_PB_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    dbg_srst_en                : 1; /* [0..0] dbg子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_DBG_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    axim_srst_en               : 1; /* [0..0] axim子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_AXIM_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    axis_srst_en               : 1; /* [0..0] axis子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0，复位期间该模块寄存器无法访问 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_AXIS_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    ahbs_srst_en               : 1; /* [0..0] ahbs子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0，复位期间该模块寄存器无法访问 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_AHBS_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    int_srst_en                : 1; /* [0..0] int子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0，复位期间该模块寄存器无法访问 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_INT_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    stu_srst_en                : 1; /* [0..0] stu子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0，复位期间该模块寄存器无法访问 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_STU_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    pdm_srst_en                : 1; /* [0..0] pdm子模块的软复位信号。复位清0电路和寄存器。置1使能复位，置0清除复位。初始值为0，复位期间该模块寄存器无法访问 */
        unsigned int    Reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_PDM_SRST_EN_T;    /* 寄存器。 */

typedef union
{
    struct
    {
        unsigned int    atpram_ctrl                : 16; /* [15..0] bbp_dma和bus2bbp模块的atpram_ctrl寄存器 */
        unsigned int    Reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_INT_ATPRAM_CTRL_T;    /* 寄存器。 */


/********************************************************************************/
/*    bbp_int 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_0,dsp_sym_int_msk_0,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_1,dsp_sym_int_msk_1,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_2,dsp_sym_int_msk_2,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_3,dsp_sym_int_msk_3,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_4,dsp_sym_int_msk_4,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_5,dsp_sym_int_msk_5,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_6,dsp_sym_int_msk_6,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_7,dsp_sym_int_msk_7,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_8,dsp_sym_int_msk_8,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_9,dsp_sym_int_msk_9,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_10,dsp_sym_int_msk_10,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_mask_11,dsp_sym_int_mask_11,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_12,dsp_sym_int_msk_12,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_dsp_sym_int_msk_13,dsp_sym_int_msk_13,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_msk_reserved,Reserved,HI_BBP_INT_DSP_SYM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_0,dsp_sym_int_cls_0,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_1,dsp_sym_int_cls_1,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_2,dsp_sym_int_cls_2,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_3,dsp_sym_int_cls_3,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_4,dsp_sym_int_cls_4,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_5,dsp_sym_int_cls_5,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_6,dsp_sym_int_cls_6,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_7,dsp_sym_int_cls_7,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_8,dsp_sym_int_cls_8,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_9,dsp_sym_int_cls_9,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_10,dsp_sym_int_cls_10,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_11,dsp_sym_int_cls_11,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_12,dsp_sym_int_cls_12,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_dsp_sym_int_cls_13,dsp_sym_int_cls_13,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_cls_reserved,Reserved,HI_BBP_INT_DSP_SYM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_0,dsp_sym_int_typ_0,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_1,dsp_sym_int_typ_1,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_2,dsp_sym_int_typ_2,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_3,dsp_sym_int_typ_3,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_4,dsp_sym_int_typ_4,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_5,dsp_sym_int_typ_5,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_6,dsp_sym_int_typ_6,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_7,dsp_sym_int_typ_7,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_8,dsp_sym_int_typ_8,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_9,dsp_sym_int_typ_9,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_10,dsp_sym_int_typ_10,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_11,dsp_sym_int_typ_11,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_12,dsp_sym_int_typ_12,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_dsp_sym_int_typ_13,dsp_sym_int_typ_13,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_typ_reserved,Reserved,HI_BBP_INT_DSP_SYM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_0,dsp_sym_int_alm_0,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_1,dsp_sym_int_alm_1,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_2,dsp_sym_int_alm_2,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_3,dsp_sym_int_alm_3,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_4,dsp_sym_int_alm_4,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_5,dsp_sym_int_alm_5,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_6,dsp_sym_int_alm_6,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_7,dsp_sym_int_alm_7,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_8,dsp_sym_int_alm_8,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_9,dsp_sym_int_alm_9,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_10,dsp_sym_int_alm_10,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_11,dsp_sym_int_alm_11,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_12,dsp_sym_int_alm_12,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_dsp_sym_int_alm_13,dsp_sym_int_alm_13,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_sym_int_alm_reserved,Reserved,HI_BBP_INT_DSP_SYM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_SYM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_cells_int_msk,dsp_cells_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_pwrm_int_msk,dsp_pwrm_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_cqi_int_msk,dsp_cqi_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_pb_int_msk,dsp_pb_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_pdu0_int_msk,dsp_pdu0_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_pdu1_int_msk,dsp_pdu1_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_pdu2_int_msk,dsp_pdu2_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_cmu_int_msk,dsp_cmu_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_vdl_int_msk,dsp_vdl_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_cfi_int_msk,dsp_cfi_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_dma_int_msk,dsp_dma_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_tdl_int_mask,dsp_tdl_int_mask,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_dcf_int_msk,dsp_dcf_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_ul_int_msk,dsp_ul_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_fpu_int_msk,dsp_fpu_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_aagc_int_msk,dsp_aagc_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_rstd_int_msk,dsp_rstd_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_dma_anten0_int_msk,dsp_dma_anten0_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_dma_anten1_int_msk,dsp_dma_anten1_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_wlan_priority_int_msk,dsp_wlan_priority_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_dsp_wlan_tx_active_int_msk,dsp_wlan_tx_active_int_msk,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_msk_reserved,Reserved,HI_BBP_INT_DSP_MOD_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_cells_int_cls,dsp_cells_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_pwrm_int_cls,dsp_pwrm_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_cqi_int_cls,dsp_cqi_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_pb_int_cls,dsp_pb_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_pdu0_int_cls,dsp_pdu0_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_pdu1_int_cls,dsp_pdu1_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_pdu2_int_cls,dsp_pdu2_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_cmu_int_cls,dsp_cmu_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_vdl_int_cls,dsp_vdl_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_cfi_int_cls,dsp_cfi_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_dma_int_cls,dsp_dma_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_tdl_int_cls,dsp_tdl_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_dcf_int_cls,dsp_dcf_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_ul_int_cls,dsp_ul_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_fpu_int_cls,dsp_fpu_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_aagc_int_cls,dsp_aagc_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_rstd_int_cls,dsp_rstd_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_dma_anten0_int_cls,dsp_dma_anten0_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_dma_anten1_int_cls,dsp_dma_anten1_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_wlan_priority_int_cls,dsp_wlan_priority_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_dsp_wlan_tx_active_int_cls,dsp_wlan_tx_active_int_cls,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_cls_reserved,Reserved,HI_BBP_INT_DSP_MOD_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_cells_int_typ,dsp_cells_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_pwrm_int_typ,dsp_pwrm_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_cqi_int_typ,dsp_cqi_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_pb_int_typ,dsp_pb_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_pdu0_int_typ,dsp_pdu0_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_pdu1_int_typ,dsp_pdu1_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_pdu2_int_typ,dsp_pdu2_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_cmu_int_typ,dsp_cmu_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_vdl_int_typ,dsp_vdl_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_cfi_int_typ,dsp_cfi_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_dma_int_typ,dsp_dma_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_tdl_int_typ,dsp_tdl_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_dcf_int_typ,dsp_dcf_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_ul_int_typ,dsp_ul_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_fpu_int_typ,dsp_fpu_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_aagc_int_typ,dsp_aagc_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_rstd_int_typ,dsp_rstd_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_dma_anten0_int_typ,dsp_dma_anten0_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_dma_anten1_int_typ,dsp_dma_anten1_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_wlan_priority_int_typ,dsp_wlan_priority_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_dsp_wlan_tx_active_int_typ,dsp_wlan_tx_active_int_typ,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_typ_reserved,Reserved,HI_BBP_INT_DSP_MOD_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_cells_int_alm,dsp_cells_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_pwrm_int_alm,dsp_pwrm_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_cqi_int_alm,dsp_cqi_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_pb_int_alm,dsp_pb_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_pdu0_int_alm,dsp_pdu0_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_pdu1_int_alm,dsp_pdu1_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_pdu2_int_alm,dsp_pdu2_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_cmu_int_alm,dsp_cmu_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_vdl_int_alm,dsp_vdl_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_cfi_int_alm,dsp_cfi_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_dma_int_alm,dsp_dma_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_tdl_int_alm,dsp_tdl_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_dcf_int_alm,dsp_dcf_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_ul_int_alm,dsp_ul_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_fpu_int_alm,dsp_fpu_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_aagc_int_alm,dsp_aagc_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_rstd_int_alm,dsp_rstd_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_dma_anten0_int_alm,dsp_dma_anten0_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_dma_anten1_int_alm,dsp_dma_anten1_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_wlan_priority_int_alm,dsp_wlan_priority_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_dsp_wlan_tx_active_int_alm,dsp_wlan_tx_active_int_alm,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_dsp_mod_int_alm_reserved,Reserved,HI_BBP_INT_DSP_MOD_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DSP_MOD_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_msk_rfin_warn_int_msk,rfin_warn_int_msk,HI_BBP_INT_WARN_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_msk_fpu_warn_int_msk,fpu_warn_int_msk,HI_BBP_INT_WARN_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_msk_ul_warn_int_msk,ul_warn_int_msk,HI_BBP_INT_WARN_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_msk_cqi_warn_int_msk,cqi_warn_int_msk,HI_BBP_INT_WARN_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_msk_pb_warn_int_msk,pb_warn_int_msk,HI_BBP_INT_WARN_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_msk_vdl_warn_int_msk,vdl_warn_int_msk,HI_BBP_INT_WARN_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_msk_pdu_warn_int_msk,pdu_warn_int_msk,HI_BBP_INT_WARN_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_msk_tdl_warn_int_msk,tdl_warn_int_msk,HI_BBP_INT_WARN_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_msk_dma_warn_int_msk,dma_warn_int_msk,HI_BBP_INT_WARN_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_msk_reserved,Reserved,HI_BBP_INT_WARN_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_cls_rfin_warn_int_cls,rfin_warn_int_cls,HI_BBP_INT_WARN_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_cls_fpu_warn_int_cls,fpu_warn_int_cls,HI_BBP_INT_WARN_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_cls_ul_warn_int_cls,ul_warn_int_cls,HI_BBP_INT_WARN_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_cls_cqi_warn_int_cls,cqi_warn_int_cls,HI_BBP_INT_WARN_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_cls_pb_warn_int_cls,pb_warn_int_cls,HI_BBP_INT_WARN_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_cls_vdl_warn_int_cls,vdl_warn_int_cls,HI_BBP_INT_WARN_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_cls_pdu_warn_int_cls,pdu_warn_int_cls,HI_BBP_INT_WARN_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_cls_tdl_warn_int_cls,tdl_warn_int_cls,HI_BBP_INT_WARN_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_cls_dma_warn_int_cls,dma_warn_int_cls,HI_BBP_INT_WARN_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_cls_reserved,Reserved,HI_BBP_INT_WARN_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_typ_rfin_warn_int_typ,rfin_warn_int_typ,HI_BBP_INT_WARN_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_typ_fpu_warn_int_typ,fpu_warn_int_typ,HI_BBP_INT_WARN_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_typ_ul_warn_int_typ,ul_warn_int_typ,HI_BBP_INT_WARN_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_typ_cqi_warn_int_typ,cqi_warn_int_typ,HI_BBP_INT_WARN_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_typ_pb_warn_int_typ,pb_warn_int_typ,HI_BBP_INT_WARN_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_typ_vdl_warn_int_typ,vdl_warn_int_typ,HI_BBP_INT_WARN_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_typ_pdu_warn_int_typ,pdu_warn_int_typ,HI_BBP_INT_WARN_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_typ_tdl_warn_int_typ,tdl_warn_int_typ,HI_BBP_INT_WARN_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_typ_dma_warn_int_typ,dma_warn_int_typ,HI_BBP_INT_WARN_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_warn_int_typ_reserved,Reserved,HI_BBP_INT_WARN_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_WARN_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_msk_arm_position_int_msk,arm_position_int_msk,HI_BBP_INT_ARM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_msk_arm_dl_dma_int_msk,arm_dl_dma_int_msk,HI_BBP_INT_ARM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_msk_reserved,Reserved,HI_BBP_INT_ARM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_cls_arm_position_int_cls,arm_position_int_cls,HI_BBP_INT_ARM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_cls_arm_dl_dma_int_cls,arm_dl_dma_int_cls,HI_BBP_INT_ARM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_cls_reserved,Reserved,HI_BBP_INT_ARM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_typ_arm_position_int_typ,arm_position_int_typ,HI_BBP_INT_ARM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_typ_arm_dl_dma_int_typ,arm_dl_dma_int_typ,HI_BBP_INT_ARM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_typ_reserved,Reserved,HI_BBP_INT_ARM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_alm_arm_position_int_alm,arm_position_int_alm,HI_BBP_INT_ARM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_alm_arm_dl_dma_int_alm,arm_dl_dma_int_alm,HI_BBP_INT_ARM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_arm_int_alm_reserved,Reserved,HI_BBP_INT_ARM_INT_ALM_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ARM_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_int_app_arm_int_msk_aarm_dbg_msk,aarm_dbg_msk,HI_BBP_INT_APP_ARM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_APP_ARM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_app_arm_int_msk_aarm_dma_msk,aarm_dma_msk,HI_BBP_INT_APP_ARM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_APP_ARM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_app_arm_int_msk_reserved,Reserved,HI_BBP_INT_APP_ARM_INT_MSK_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_APP_ARM_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_int_app_arm_int_cls_aarm_dbg_cls,aarm_dbg_cls,HI_BBP_INT_APP_ARM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_APP_ARM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_app_arm_int_cls_aarm_dma_cls,aarm_dma_cls,HI_BBP_INT_APP_ARM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_APP_ARM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_app_arm_int_cls_reserved,Reserved,HI_BBP_INT_APP_ARM_INT_CLS_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_APP_ARM_INT_CLS_OFFSET)
HI_SET_GET(hi_bbp_int_app_arm_int_typ_aarm_dbg_typ,aarm_dbg_typ,HI_BBP_INT_APP_ARM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_APP_ARM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_app_arm_int_typ_aarm_dma_typ,aarm_dma_typ,HI_BBP_INT_APP_ARM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_APP_ARM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_app_arm_int_typ_reserved,Reserved,HI_BBP_INT_APP_ARM_INT_TYP_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_APP_ARM_INT_TYP_OFFSET)
HI_SET_GET(hi_bbp_int_prior_level_prior_level,prior_level,HI_BBP_INT_PRIOR_LEVEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_PRIOR_LEVEL_OFFSET)
HI_SET_GET(hi_bbp_int_prior_level_reserved,Reserved,HI_BBP_INT_PRIOR_LEVEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_PRIOR_LEVEL_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak1_xs_rs_gated_en,xs_rs_gated_en,HI_BBP_INT_INT2OUT_BAK1_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK1_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak1_hs_rs_gated_en,hs_rs_gated_en,HI_BBP_INT_INT2OUT_BAK1_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK1_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak1_xm_gated_en,xm_gated_en,HI_BBP_INT_INT2OUT_BAK1_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK1_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak1_xs_gs_gated_en,xs_gs_gated_en,HI_BBP_INT_INT2OUT_BAK1_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK1_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak1_hs_gs_gated_en,hs_gs_gated_en,HI_BBP_INT_INT2OUT_BAK1_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK1_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak1_int2out_bak1,int2out_bak1,HI_BBP_INT_INT2OUT_BAK1_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK1_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak1_reserved,Reserved,HI_BBP_INT_INT2OUT_BAK1_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK1_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak2_int2out_bak2,int2out_bak2,HI_BBP_INT_INT2OUT_BAK2_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK2_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak2_reserved,Reserved,HI_BBP_INT_INT2OUT_BAK2_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK2_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak3_int2out_bak3,int2out_bak3,HI_BBP_INT_INT2OUT_BAK3_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK3_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak3_reserved,Reserved,HI_BBP_INT_INT2OUT_BAK3_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK3_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt0_int_typ,dt0_int_typ,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt1_int_typ,dt1_int_typ,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt2_int_typ,dt2_int_typ,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt3_int_typ,dt3_int_typ,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt4_int_typ,dt4_int_typ,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt5_int_typ,dt5_int_typ,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt6_int_typ,dt6_int_typ,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt7_int_typ,dt7_int_typ,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt0_int_alm,dt0_int_alm,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt1_int_alm,dt1_int_alm,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt2_int_alm,dt2_int_alm,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt3_int_alm,dt3_int_alm,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt4_int_alm,dt4_int_alm,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt5_int_alm,dt5_int_alm,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt6_int_alm,dt6_int_alm,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_dt7_int_alm,dt7_int_alm,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak4_reserved,Reserved,HI_BBP_INT_INT2OUT_BAK4_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK4_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak5_reserved_1,Reserved_1,HI_BBP_INT_INT2OUT_BAK5_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK5_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak5_dt_int_msk,dt_int_msk,HI_BBP_INT_INT2OUT_BAK5_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK5_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak5_reserved_0,Reserved_0,HI_BBP_INT_INT2OUT_BAK5_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK5_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak6_int2out_bak6,int2out_bak6,HI_BBP_INT_INT2OUT_BAK6_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK6_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak6_reserved,Reserved,HI_BBP_INT_INT2OUT_BAK6_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK6_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak7_int2out_bak7,int2out_bak7,HI_BBP_INT_INT2OUT_BAK7_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK7_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak7_reserved,Reserved,HI_BBP_INT_INT2OUT_BAK7_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK7_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak8_int2out_bak8,int2out_bak8,HI_BBP_INT_INT2OUT_BAK8_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK8_OFFSET)
HI_SET_GET(hi_bbp_int_int2out_bak8_reserved,Reserved,HI_BBP_INT_INT2OUT_BAK8_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT2OUT_BAK8_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_reserved_4,Reserved_4,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_reserved_3,Reserved_3,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_rfin_clk_sel,rfin_clk_sel,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_fpu_clk_sel,fpu_clk_sel,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_ul_clk_sel,ul_clk_sel,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_vdl_clk_sel,vdl_clk_sel,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_pb_clk_sel,pb_clk_sel,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_dbg_clk_sel,dbg_clk_sel,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_axim_clk_sel,axim_clk_sel,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_axis_clk_sel,axis_clk_sel,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_ahbs_clk_sel,ahbs_clk_sel,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_reserved_2,Reserved_2,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_reserved_1,Reserved_1,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_pdm_clk_sel,pdm_clk_sel,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_clk_en_sel_reserved_0,Reserved_0,HI_BBP_INT_CLK_EN_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_CLK_EN_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_ahb2lbus_clk_sel_ahb2lbus_clk_sel,ahb2lbus_clk_sel,HI_BBP_INT_AHB2LBUS_CLK_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_AHB2LBUS_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_ahb2lbus_clk_sel_reserved,Reserved,HI_BBP_INT_AHB2LBUS_CLK_SEL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_AHB2LBUS_CLK_SEL_OFFSET)
HI_SET_GET(hi_bbp_int_rfin_srst_en_rfin_srst_en,rfin_srst_en,HI_BBP_INT_RFIN_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_RFIN_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_rfin_srst_en_reserved,Reserved,HI_BBP_INT_RFIN_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_RFIN_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_fpu_srst_en_fpu_srst_en,fpu_srst_en,HI_BBP_INT_FPU_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_FPU_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_fpu_srst_en_reserved,Reserved,HI_BBP_INT_FPU_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_FPU_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_ul_srst_en_ul_srst_en,ul_srst_en,HI_BBP_INT_UL_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_UL_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_ul_srst_en_reserved,Reserved,HI_BBP_INT_UL_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_UL_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_vdl_srst_en_vdl_srst_en,vdl_srst_en,HI_BBP_INT_VDL_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_VDL_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_vdl_srst_en_reserved,Reserved,HI_BBP_INT_VDL_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_VDL_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_pb_srst_en_pb_srst_en,pb_srst_en,HI_BBP_INT_PB_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_PB_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_pb_srst_en_reserved,Reserved,HI_BBP_INT_PB_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_PB_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_dbg_srst_en_dbg_srst_en,dbg_srst_en,HI_BBP_INT_DBG_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DBG_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_dbg_srst_en_reserved,Reserved,HI_BBP_INT_DBG_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_DBG_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_axim_srst_en_axim_srst_en,axim_srst_en,HI_BBP_INT_AXIM_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_AXIM_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_axim_srst_en_reserved,Reserved,HI_BBP_INT_AXIM_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_AXIM_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_axis_srst_en_axis_srst_en,axis_srst_en,HI_BBP_INT_AXIS_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_AXIS_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_axis_srst_en_reserved,Reserved,HI_BBP_INT_AXIS_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_AXIS_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_ahbs_srst_en_ahbs_srst_en,ahbs_srst_en,HI_BBP_INT_AHBS_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_AHBS_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_ahbs_srst_en_reserved,Reserved,HI_BBP_INT_AHBS_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_AHBS_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_int_srst_en_int_srst_en,int_srst_en,HI_BBP_INT_INT_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_int_srst_en_reserved,Reserved,HI_BBP_INT_INT_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_INT_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_stu_srst_en_stu_srst_en,stu_srst_en,HI_BBP_INT_STU_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_STU_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_stu_srst_en_reserved,Reserved,HI_BBP_INT_STU_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_STU_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_pdm_srst_en_pdm_srst_en,pdm_srst_en,HI_BBP_INT_PDM_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_PDM_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_pdm_srst_en_reserved,Reserved,HI_BBP_INT_PDM_SRST_EN_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_PDM_SRST_EN_OFFSET)
HI_SET_GET(hi_bbp_int_atpram_ctrl_atpram_ctrl,atpram_ctrl,HI_BBP_INT_ATPRAM_CTRL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ATPRAM_CTRL_OFFSET)
HI_SET_GET(hi_bbp_int_atpram_ctrl_reserved,Reserved,HI_BBP_INT_ATPRAM_CTRL_T,HI_BBP_INT_BASE_ADDR, HI_BBP_INT_ATPRAM_CTRL_OFFSET)

#endif

#endif // __HI_BBP_INT_H__

