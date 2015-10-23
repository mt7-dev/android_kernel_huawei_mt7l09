/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_bbp_tstu.h*/
/* Version       : 2.0 */
/* Created       : 2013-09-13*/
/* Last Modified : */
/* Description   :  The C union definition file for the module bbp_tstu*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_BBP_TSTU_H__
#define __HI_BBP_TSTU_H__

/*
 * Project: hi
 * Module : bbp_tstu
 */

#ifndef HI_SET_GET
#define HI_SET_GET(a0,a1,a2,a3,a4)
#endif

/********************************************************************************/
/*    bbp_tstu 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_BBP_TSTU_SOFT_RST_OFFSET                       (0x0) /* 软复位 */
#define    HI_BBP_TSTU_SFN_OFFSET                            (0x10) /* 子帧号寄存器 */
#define    HI_BBP_TSTU_SFN_OFT_OFFSET                        (0x14) /* 子帧号偏移寄存器 */
#define    HI_BBP_TSTU_SFRM_FRAML_OFFSET                     (0x18) /* 子帧长度寄存器 */
#define    HI_BBP_TSTU_SFRM_FRAML_SET_OFFSET                 (0x1C) /* 子帧长度调整寄存器 */
#define    HI_BBP_TSTU_SLOT_INT_POS_OFFSET                   (0x20) /* DSP时隙中断位置寄存器 */
#define    HI_BBP_TSTU_TIME_ADJ_START_OFFSET                 (0x24) /* 定时调整使能 */
#define    HI_BBP_TSTU_TIME_ADJ_ALM_OFFSET                   (0x28) /* 定时调整告警 */
#define    HI_BBP_TSTU_DBG_TIME_SEL_OFFSET                   (0x2C)
#define    HI_BBP_TSTU_CLK_MSR_PRD_OFFSET                    (0x40) /* 时钟测量周期 */
#define    HI_BBP_TSTU_CLK_STATE_CLEAR_OFFSET                (0x44) /* 时钟测量状态清除 */
#define    HI_BBP_TSTU_CLK_MSR_START_OFFSET                  (0x48) /* 时钟测量使能 */
#define    HI_BBP_TSTU_LAYOUT_TIME_OFFSET                    (0x70) /* 任务规划时间寄存器 */
#define    HI_BBP_TSTU_LAYOUT_CHIP_OFFSET                    (0x74) /* 任务规划时间控制寄存器 */
#define    HI_BBP_TSTU_LAYOUT_START_OFFSET                   (0x78) /* 任务规划控制寄存器 */
#define    HI_BBP_TSTU_PUB_INT_MSK_OFFSET                    (0xA0) /* 子帧中断屏蔽寄存器 */
#define    HI_BBP_TSTU_DSP_INT_MSK_OFFSET                    (0xA4) /* DSP时隙中断屏蔽寄存器 */
#define    HI_BBP_TSTU_PUB_INT_CLEAR_OFFSET                  (0xA8) /* 子帧中断清除寄存器 */
#define    HI_BBP_TSTU_DSP_INT_CLEAR_OFFSET                  (0xAC) /* DSP时隙中断清除寄存器 */
#define    HI_BBP_TSTU_PUB_INT_ALM_OFFSET                    (0xB0) /* 子帧中断告警寄存器 */
#define    HI_BBP_TSTU_DSP_INT_ALM_OFFSET                    (0xB4) /* DSP时隙中断告警寄存器 */
#define    HI_BBP_TSTU_STU_CFG1_OFFSET                       (0x100) /* 保留寄存器 */
#define    HI_BBP_TSTU_STU_CFG2_OFFSET                       (0x104) /* 保留寄存器 */
#define    HI_BBP_TSTU_STU_CFG3_OFFSET                       (0x108) /* 保留寄存器 */
#define    HI_BBP_TSTU_STU_CFG4_OFFSET                       (0x10C) /* 保留寄存器 */
#define    HI_BBP_TSTU_TDS_STU_FRAMC_OFFSET                  (0x200) /* 子帧内计数器 */
#define    HI_BBP_TSTU_TDS_STU_SFN_OFFSET                    (0x204) /* 子帧号 */
#define    HI_BBP_TSTU_TDS_STU_FRAML_OFFSET                  (0x208) /* 当前子帧长度 */
#define    HI_BBP_TSTU_TDS_STU_SLOT_OFFSET                   (0x20C) /* 当前时隙号 */
#define    HI_BBP_TSTU_CLK_MSR_CNT_OFFSET                    (0x220) /* 时钟测量结果 */
#define    HI_BBP_TSTU_CLK_MSR_STATE_OFFSET                  (0x224) /* 时钟测量状态 */
#define    HI_BBP_TSTU_LAYOUT_CUR_TIME_OFFSET                (0x230) /* 当前LAYOUT剩余时间 */
#define    HI_BBP_TSTU_LAYOUT_STATE_OFFSET                   (0x234) /* 任务规划状态寄存器 */
#define    HI_BBP_TSTU_PUB_INT_STATE_OFFSET                  (0x240) /* 子帧中断状态寄存器 */
#define    HI_BBP_TSTU_DSP_INT_STATE_OFFSET                  (0x244) /* DSP时隙中断状态寄存器 */
#define    HI_BBP_TSTU_DBG_SNF_PRE_STU_OFFSET                (0x300) /* 进入睡眠前锁存的stu的子帧号寄存器 */
#define    HI_BBP_TSTU_DBG_SFN_POST_OFFSET                   (0x304) /* 退出睡眠后锁存的drx和stu子帧号寄存器 */
#define    HI_BBP_TSTU_DBG_FRAMC_PRE_STU_OFFSET              (0x308) /* 进入睡眠前锁存的stu子帧内计数器寄存器 */
#define    HI_BBP_TSTU_DBG_FRAMC_POST_DRX_OFFSET             (0x30C) /* 退出睡眠后锁存的drx子帧内计数器寄存器 */
#define    HI_BBP_TSTU_DBG_FRAMC_POST_STU_OFFSET             (0x310) /* 退出睡眠后锁存的stu子帧内计数器寄存器 */

/********************************************************************************/
/*    bbp_tstu 寄存器定义（项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    soft_rst                   : 1; /* [0..0] STU模块软复位寄存器：1'b1，软复位有效配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_SOFT_RST_T;    /* 软复位 */

typedef union
{
    struct
    {
        unsigned int    sfn                        : 16; /* [15..0] 配置子帧号寄存器。软件配置后，子帧号计数器将以该寄存器为初值进行子帧号的累加。特别注意，该寄存器不是系统帧号配置时刻：配置定时调整使能前生效时刻：定时调整使能有效并且该使能有效时，当前子帧结束后子帧号立即修改为配置值，仅生效一次 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_SFN_T;    /* 子帧号寄存器 */

typedef union
{
    struct
    {
        unsigned int    sfn_oft                    : 16; /* [15..0] 子帧号偏移寄存器，通过该寄存器可调整子帧号。若希望调整偏移量为B，则该寄存器应配置为B+1配置时刻：配置定时调整是能前生效时刻：定时调整时，当前子帧结束时，该值被累加到当前子帧号上，仅生效一次 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_SFN_OFT_T;    /* 子帧号偏移寄存器 */

typedef union
{
    struct
    {
        unsigned int    sfrm_framl                 : 20; /* [19..0] TDS 5ms子帧长度。该寄存器值为1/96 chip精度，默认值为5ms子帧长度减1，即614339，支持软件可配置。配置时刻：任意时刻生效时刻：子帧中断，下一子帧 */
        unsigned int    reserved                   : 12; /* [31..20] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_SFRM_FRAML_T;    /* 子帧长度寄存器 */

typedef union
{
    struct
    {
        unsigned int    sfrm_framl_set             : 20; /* [19..0] STD子帧长度调整寄存器，通过该寄存器调整当前子帧长度。当配置定时调整使能时，如果配置的该寄存器的值小于等于当前子帧内计数器的值，当前子帧长度保持为SFRM_FRAML，否则子帧长度立即调整为该寄存器的值，1/96 chip精度。配置的值为真实值减1。配置时刻：配置定时调整是能前生效时刻：定时调整使能有效时，立即生效 */
        unsigned int    reserved                   : 12; /* [31..20] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_SFRM_FRAML_SET_T;    /* 子帧长度调整寄存器 */

typedef union
{
    struct
    {
        unsigned int    spec_slot_int_pos          : 16; /* [15..0] 特殊时隙中断位置寄存器。时隙中断默认位置在1/3、2/3时隙位置。支持默认值位置前后可调1/16时隙，即寄存器配置范围为9151~13375，1/96chip精度配置时刻：任意生效时刻：立即 */
        unsigned int    norm_slot_int_pos          : 16; /* [31..16] 普通时隙中断位置寄存器。时隙中断默认位置在1/4、1/2、3/4时隙位置。支持默认值位置前后可调1/16时隙，即寄存器配置范围为15551~25919，1/96chip精度配置时刻：任意生效时刻：立即 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_SLOT_INT_POS_T;    /* DSP时隙中断位置寄存器 */

typedef union
{
    struct
    {
        unsigned int    time_track_start           : 1; /* [0..0] 定时跟踪启动信号：1'b1，启动定时切换；否则无效。定时跟踪时配置该寄存器。在定时跟踪时，无需屏蔽中断配置时刻：任意时刻生效时刻：立即，逻辑需切到新的子帧头位置 */
        unsigned int    time_switch_start          : 1; /* [1..1] 定时切换启动信号：1'b1，启动定时切换；否则无效。定时切换时配置该寄存器，用以屏蔽定时切换过程中的中断配置时刻：任意时刻生效时刻：立即，逻辑需切到新的子帧头位置 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_TIME_ADJ_START_T;    /* 定时调整使能 */

typedef union
{
    struct
    {
        unsigned int    time_adj_alm               : 1; /* [0..0] 定时调整告警寄存器。通过配置sfrm_len_set寄存器来调整定时时，如果当前子帧内计数器值大于配置值，此次配置失效，置该位为1'b1。向该位写入1'b1可清除告警配置时刻：任意时刻生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_TIME_ADJ_ALM_T;    /* 定时调整告警 */

typedef union
{
    struct
    {
        unsigned int    dbg_time_sel               : 1; /* [0..0]  */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_DBG_TIME_SEL_T;

typedef union
{
    struct
    {
        unsigned int    clk_msr_prd                : 16; /* [15..0] 时钟测量周期，32.768KHz时钟个数，大于0，相当于xxms。默认值为2048，若寄存器配置为0，并且启动时钟测量，逻辑不会开启测量功能配置时刻：开始时钟测量前配置生效时刻：立即 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_CLK_MSR_PRD_T;    /* 时钟测量周期 */

typedef union
{
    struct
    {
        unsigned int    clk_state_clear            : 1; /* [0..0] 时钟测量状态清除。向该寄存器写入1'b1清除时钟测量状态。配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_CLK_STATE_CLEAR_T;    /* 时钟测量状态清除 */

typedef union
{
    struct
    {
        unsigned int    clk_msr_start              : 1; /* [0..0] 时钟测量启动信号：1'b1，启动时钟测量。配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_CLK_MSR_START_T;    /* 时钟测量使能 */

typedef union
{
    struct
    {
        unsigned int    layout_time                : 32; /* [31..0] 任务规划时间寄存器配置时刻：配置任务规划使能前生效时刻：立即 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_LAYOUT_TIME_T;    /* 任务规划时间寄存器 */

typedef union
{
    struct
    {
        unsigned int    layout_chip                : 15; /* [14..0] 任务规划时间控制寄存器，单位为chip数，该寄存器的值决定LAYOUT_TIMER的递减周期配置时刻：配置任务规划使能前生效时刻：立即 */
        unsigned int    reserved                   : 17; /* [31..15] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_LAYOUT_CHIP_T;    /* 任务规划时间控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    layout_start               : 1; /* [0..0] 任务规划启动寄存器：1'b1，启动任务规划配置时刻：任意时刻生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_LAYOUT_START_T;    /* 任务规划控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    hifi_sfrm_int_msk          : 1; /* [0..0] hifi子帧中断屏蔽：1'b1时，STU模块产生子帧中断，并上报配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    arm_sfrm_int_msk           : 1; /* [1..1] arm子帧中断屏蔽：1'b1时，STU模块产生子帧中断，并上报配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_PUB_INT_MSK_T;    /* 子帧中断屏蔽寄存器 */

typedef union
{
    struct
    {
        unsigned int    slot_int_msk               : 23; /* [22..0] DSP时隙中断屏蔽：1'b1时，打开时隙中断：bit22:bit21，分别为特殊时隙的2/3和1/3中断bit20:bit0，普通时隙0~6的中断屏蔽，其中每连续3-bit为一组，共7组，bit位由低到高分别表示对用时隙的1/4、1/2、3/中断屏蔽位。配置时刻：任意时刻生效时刻：立即 */
        unsigned int    dsp_sfrm_int_msk           : 1; /* [23..23] DSP子帧中断屏蔽：1'b1时，STU模块产生子帧中断，并上报配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    reserved                   : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_DSP_INT_MSK_T;    /* DSP时隙中断屏蔽寄存器 */

typedef union
{
    struct
    {
        unsigned int    hifi_sfrm_int_clear        : 1; /* [0..0] hifi子帧中断清除信号，1'b1，清除子帧中断：配置时刻：任意时刻生效时刻：立即 */
        unsigned int    arm_sfrm_int_clear         : 1; /* [1..1] arm子帧中断清除信号，1'b1，清除子帧中断：配置时刻：任意时刻生效时刻：立即 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_PUB_INT_CLEAR_T;    /* 子帧中断清除寄存器 */

typedef union
{
    struct
    {
        unsigned int    slot_int_clear             : 23; /* [22..0] DSP时隙中断清除：1'b1时，清除时隙中断：bit22:bit21，分别为特殊时隙的2/3和1/3中断清除bit20:bit0，普通时隙0~6的中断清除，其中每连续3-bit为一组，共7组，bit位由低到高分别表示对用时隙的1/4、1/2、3/中断清除位。配置时刻：任意时刻生效时刻：立即 */
        unsigned int    dsp_sfrm_int_clear         : 1; /* [23..23] DSP子帧中断清除信号，1'b1，清除子帧中断：配置时刻：任意时刻生效时刻：立即 */
        unsigned int    reserved                   : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_DSP_INT_CLEAR_T;    /* DSP时隙中断清除寄存器 */

typedef union
{
    struct
    {
        unsigned int    hifi_sfrm_int_alm          : 1; /* [0..0] hifi子帧中断告警寄存器。当新中断到来而原来的中断还未被清除时，产生告警信号，置该位为1'b1。向该位写入1'b1可清除告警配置时刻：任意时刻生效时刻：立即 */
        unsigned int    arm_sfrm_int_alm           : 1; /* [1..1] arm子帧中断告警寄存器。当新中断到来而原来的中断还未被清除时，产生告警信号，置该位为1'b1。向该位写入1'b1可清除告警配置时刻：任意时刻生效时刻：立即 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_PUB_INT_ALM_T;    /* 子帧中断告警寄存器 */

typedef union
{
    struct
    {
        unsigned int    slot_int_alm               : 23; /* [22..0] 时隙中断告警寄存器。当新中断到来而之前的中断还未被清除时，产生告警，置该位为1'b1。向该位写入1'b1可清除告警位。bit22:bit21，分别为特殊时隙的2/3和1/3中告警bit20:bit0，普通时隙0~6的中断告警，其中每连续3-bit为一组，共7组，bit位由低到高分别表示对用时隙的1/4、1/2、3/中断告警位。配置时刻：任意时刻生效时刻：立即 */
        unsigned int    dsp_sfrm_int_alm           : 1; /* [23..23] DSP子帧中断告警寄存器。当新中断到来而原来的中断还未被清除时，产生告警信号，置该位为1'b1。向该位写入1'b1可清除告警配置时刻：任意时刻生效时刻：立即 */
        unsigned int    reserved                   : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_DSP_INT_ALM_T;    /* DSP时隙中断告警寄存器 */

typedef union
{
    struct
    {
        unsigned int    stu_cfg1                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_STU_CFG1_T;    /* 保留寄存器 */

typedef union
{
    struct
    {
        unsigned int    stu_cfg2                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_STU_CFG2_T;    /* 保留寄存器 */

typedef union
{
    struct
    {
        unsigned int    stu_cfg3                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_STU_CFG3_T;    /* 保留寄存器 */

typedef union
{
    struct
    {
        unsigned int    stu_cfg4                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_STU_CFG4_T;    /* 保留寄存器 */

typedef union
{
    struct
    {
        unsigned int    tds_stu_framc              : 20; /* [19..0] TDS 5ms子帧内计数器，1/96chip精度 */
        unsigned int    reserved                   : 12; /* [31..20] reserved */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_TDS_STU_FRAMC_T;    /* 子帧内计数器 */

typedef union
{
    struct
    {
        unsigned int    tds_stu_sfn                : 16; /* [15..0] stu维护的子帧号 */
        unsigned int    reserved                   : 16; /* [31..16] reserved */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_TDS_STU_SFN_T;    /* 子帧号 */

typedef union
{
    struct
    {
        unsigned int    tds_stu_framl              : 20; /* [19..0] TDS STU当前的子帧长度，1/96 chip精度 */
        unsigned int    reserved                   : 12; /* [31..20] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_TDS_STU_FRAML_T;    /* 当前子帧长度 */

typedef union
{
    struct
    {
        unsigned int    tds_stu_slot               : 3; /* [2..0] STU当前的时隙号：4'h7，时隙6,4'h6，时隙5,4'h5，时隙4,4'h4，时隙3,4'h3，时隙2,4'h2，时隙1,4'h1，特殊时隙,4'h0，时隙0 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_TDS_STU_SLOT_T;    /* 当前时隙号 */

typedef union
{
    struct
    {
        unsigned int    clk_msr_cnt                : 32; /* [31..0] 在时钟测量周期内,系统时钟的计数个数。为了保证不出现读写冲突，DSP在读取这个寄存器时必须连续读取两次，若两次结果一致则认为读取正确，否则重新读取。 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_CLK_MSR_CNT_T;    /* 时钟测量结果 */

typedef union
{
    struct
    {
        unsigned int    clk_msr_state              : 1; /* [0..0] 时钟测量状态指示：1'b1，测量结果可用；1'b0，测量结果不可用。时钟测量过程中，测量结果不可用 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_CLK_MSR_STATE_T;    /* 时钟测量状态 */

typedef union
{
    struct
    {
        unsigned int    layout_cur_time            : 32; /* [31..0] 当前任务规划剩余时间 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_LAYOUT_CUR_TIME_T;    /* 当前LAYOUT剩余时间 */

typedef union
{
    struct
    {
        unsigned int    layout_state               : 1; /* [0..0] 当前任务规划状态指示：1'b1，表示任务规划正在进行1'b0，表示任务规划已完成 */
        unsigned int    reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_LAYOUT_STATE_T;    /* 任务规划状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    hifi_sfrm_int_state        : 1; /* [0..0] hifi子帧中断状态寄存器。向中断清除寄存器写入1'b1可清除中断状态 */
        unsigned int    arm_sfrm_int_state         : 1; /* [1..1] arm子帧中断状态寄存器。向中断清除寄存器写入1'b1可清除中断状态 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_PUB_INT_STATE_T;    /* 子帧中断状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    slot_int_state             : 23; /* [22..0] 时隙中断状态寄存器。向中断清除寄存器写入1'b1可清除中断状态bit22:bit21，分别为特殊时隙2/3、1/3中断状态位bit20:bit0，普通时隙0~6的中断状态，其中每连续3-bit为一组，共7组，bit位由低到高分别表示对用时隙的1/4、1/2、3/中断状态位 */
        unsigned int    dsp_sfrm_int_state         : 1; /* [23..23] DSP子帧中断状态寄存器。向中断清除寄存器写入1'b1可清除中断状态 */
        unsigned int    reserved                   : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_DSP_INT_STATE_T;    /* DSP时隙中断状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_sfn_pre_stu            : 16; /* [15..0] 进入睡眠时锁存的stu的子帧号 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_DBG_SNF_PRE_STU_T;    /* 进入睡眠前锁存的stu的子帧号寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_sfn_post_stu           : 16; /* [15..0] 退出睡眠时锁存的stu的子帧号 */
        unsigned int    dbg_sfn_post_drx           : 16; /* [31..16] 退出睡眠时锁存的drx的子帧号 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_DBG_SFN_POST_T;    /* 退出睡眠后锁存的drx和stu子帧号寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_framc_pre_stu          : 20; /* [19..0] 进入睡眠时锁存的stu的子帧内计数器值，1/96 chip精度 */
        unsigned int    reserved                   : 12; /* [31..20] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_DBG_FRAMC_PRE_STU_T;    /* 进入睡眠前锁存的stu子帧内计数器寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_framc_post_drx         : 20; /* [19..0] 退出睡眠时锁存的drx的子帧内计数器值，1/96 chip精度 */
        unsigned int    reserved                   : 12; /* [31..20] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_DBG_FRAMC_POST_DRX_T;    /* 退出睡眠后锁存的drx子帧内计数器寄存器 */

typedef union
{
    struct
    {
        unsigned int    dbg_framc_post_stu         : 20; /* [19..0] 退出睡眠时锁存的stu的子帧内计数器值，1/96 chip精度 */
        unsigned int    reserved                   : 12; /* [31..20] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TSTU_DBG_FRAMC_POST_STU_T;    /* 退出睡眠后锁存的stu子帧内计数器寄存器 */


/********************************************************************************/
/*    bbp_tstu 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
HI_SET_GET(hi_bbp_tstu_soft_rst_soft_rst,soft_rst,HI_BBP_TSTU_SOFT_RST_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SOFT_RST_OFFSET)
HI_SET_GET(hi_bbp_tstu_soft_rst_reserved,reserved,HI_BBP_TSTU_SOFT_RST_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SOFT_RST_OFFSET)
HI_SET_GET(hi_bbp_tstu_sfn_sfn,sfn,HI_BBP_TSTU_SFN_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SFN_OFFSET)
HI_SET_GET(hi_bbp_tstu_sfn_reserved,reserved,HI_BBP_TSTU_SFN_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SFN_OFFSET)
HI_SET_GET(hi_bbp_tstu_sfn_oft_sfn_oft,sfn_oft,HI_BBP_TSTU_SFN_OFT_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SFN_OFT_OFFSET)
HI_SET_GET(hi_bbp_tstu_sfn_oft_reserved,reserved,HI_BBP_TSTU_SFN_OFT_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SFN_OFT_OFFSET)
HI_SET_GET(hi_bbp_tstu_sfrm_framl_sfrm_framl,sfrm_framl,HI_BBP_TSTU_SFRM_FRAML_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SFRM_FRAML_OFFSET)
HI_SET_GET(hi_bbp_tstu_sfrm_framl_reserved,reserved,HI_BBP_TSTU_SFRM_FRAML_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SFRM_FRAML_OFFSET)
HI_SET_GET(hi_bbp_tstu_sfrm_framl_set_sfrm_framl_set,sfrm_framl_set,HI_BBP_TSTU_SFRM_FRAML_SET_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SFRM_FRAML_SET_OFFSET)
HI_SET_GET(hi_bbp_tstu_sfrm_framl_set_reserved,reserved,HI_BBP_TSTU_SFRM_FRAML_SET_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SFRM_FRAML_SET_OFFSET)
HI_SET_GET(hi_bbp_tstu_slot_int_pos_spec_slot_int_pos,spec_slot_int_pos,HI_BBP_TSTU_SLOT_INT_POS_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SLOT_INT_POS_OFFSET)
HI_SET_GET(hi_bbp_tstu_slot_int_pos_norm_slot_int_pos,norm_slot_int_pos,HI_BBP_TSTU_SLOT_INT_POS_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_SLOT_INT_POS_OFFSET)
HI_SET_GET(hi_bbp_tstu_time_adj_start_time_track_start,time_track_start,HI_BBP_TSTU_TIME_ADJ_START_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TIME_ADJ_START_OFFSET)
HI_SET_GET(hi_bbp_tstu_time_adj_start_time_switch_start,time_switch_start,HI_BBP_TSTU_TIME_ADJ_START_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TIME_ADJ_START_OFFSET)
HI_SET_GET(hi_bbp_tstu_time_adj_start_reserved,reserved,HI_BBP_TSTU_TIME_ADJ_START_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TIME_ADJ_START_OFFSET)
HI_SET_GET(hi_bbp_tstu_time_adj_alm_time_adj_alm,time_adj_alm,HI_BBP_TSTU_TIME_ADJ_ALM_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TIME_ADJ_ALM_OFFSET)
HI_SET_GET(hi_bbp_tstu_time_adj_alm_reserved,reserved,HI_BBP_TSTU_TIME_ADJ_ALM_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TIME_ADJ_ALM_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_time_sel_dbg_time_sel,dbg_time_sel,HI_BBP_TSTU_DBG_TIME_SEL_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_TIME_SEL_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_time_sel_reserved,reserved,HI_BBP_TSTU_DBG_TIME_SEL_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_TIME_SEL_OFFSET)
HI_SET_GET(hi_bbp_tstu_clk_msr_prd_clk_msr_prd,clk_msr_prd,HI_BBP_TSTU_CLK_MSR_PRD_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_CLK_MSR_PRD_OFFSET)
HI_SET_GET(hi_bbp_tstu_clk_msr_prd_reserved,reserved,HI_BBP_TSTU_CLK_MSR_PRD_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_CLK_MSR_PRD_OFFSET)
HI_SET_GET(hi_bbp_tstu_clk_state_clear_clk_state_clear,clk_state_clear,HI_BBP_TSTU_CLK_STATE_CLEAR_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_CLK_STATE_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tstu_clk_state_clear_reserved,reserved,HI_BBP_TSTU_CLK_STATE_CLEAR_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_CLK_STATE_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tstu_clk_msr_start_clk_msr_start,clk_msr_start,HI_BBP_TSTU_CLK_MSR_START_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_CLK_MSR_START_OFFSET)
HI_SET_GET(hi_bbp_tstu_clk_msr_start_reserved,reserved,HI_BBP_TSTU_CLK_MSR_START_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_CLK_MSR_START_OFFSET)
HI_SET_GET(hi_bbp_tstu_layout_time_layout_time,layout_time,HI_BBP_TSTU_LAYOUT_TIME_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_LAYOUT_TIME_OFFSET)
HI_SET_GET(hi_bbp_tstu_layout_chip_layout_chip,layout_chip,HI_BBP_TSTU_LAYOUT_CHIP_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_LAYOUT_CHIP_OFFSET)
HI_SET_GET(hi_bbp_tstu_layout_chip_reserved,reserved,HI_BBP_TSTU_LAYOUT_CHIP_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_LAYOUT_CHIP_OFFSET)
HI_SET_GET(hi_bbp_tstu_layout_start_layout_start,layout_start,HI_BBP_TSTU_LAYOUT_START_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_LAYOUT_START_OFFSET)
HI_SET_GET(hi_bbp_tstu_layout_start_reserved,reserved,HI_BBP_TSTU_LAYOUT_START_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_LAYOUT_START_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_msk_hifi_sfrm_int_msk,hifi_sfrm_int_msk,HI_BBP_TSTU_PUB_INT_MSK_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_msk_arm_sfrm_int_msk,arm_sfrm_int_msk,HI_BBP_TSTU_PUB_INT_MSK_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_msk_reserved,reserved,HI_BBP_TSTU_PUB_INT_MSK_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_msk_slot_int_msk,slot_int_msk,HI_BBP_TSTU_DSP_INT_MSK_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_msk_dsp_sfrm_int_msk,dsp_sfrm_int_msk,HI_BBP_TSTU_DSP_INT_MSK_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_msk_reserved,reserved,HI_BBP_TSTU_DSP_INT_MSK_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_clear_hifi_sfrm_int_clear,hifi_sfrm_int_clear,HI_BBP_TSTU_PUB_INT_CLEAR_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_clear_arm_sfrm_int_clear,arm_sfrm_int_clear,HI_BBP_TSTU_PUB_INT_CLEAR_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_clear_reserved,reserved,HI_BBP_TSTU_PUB_INT_CLEAR_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_clear_slot_int_clear,slot_int_clear,HI_BBP_TSTU_DSP_INT_CLEAR_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_clear_dsp_sfrm_int_clear,dsp_sfrm_int_clear,HI_BBP_TSTU_DSP_INT_CLEAR_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_clear_reserved,reserved,HI_BBP_TSTU_DSP_INT_CLEAR_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_alm_hifi_sfrm_int_alm,hifi_sfrm_int_alm,HI_BBP_TSTU_PUB_INT_ALM_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_alm_arm_sfrm_int_alm,arm_sfrm_int_alm,HI_BBP_TSTU_PUB_INT_ALM_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_alm_reserved,reserved,HI_BBP_TSTU_PUB_INT_ALM_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_alm_slot_int_alm,slot_int_alm,HI_BBP_TSTU_DSP_INT_ALM_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_alm_dsp_sfrm_int_alm,dsp_sfrm_int_alm,HI_BBP_TSTU_DSP_INT_ALM_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_alm_reserved,reserved,HI_BBP_TSTU_DSP_INT_ALM_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tstu_stu_cfg1_stu_cfg1,stu_cfg1,HI_BBP_TSTU_STU_CFG1_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_STU_CFG1_OFFSET)
HI_SET_GET(hi_bbp_tstu_stu_cfg2_stu_cfg2,stu_cfg2,HI_BBP_TSTU_STU_CFG2_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_STU_CFG2_OFFSET)
HI_SET_GET(hi_bbp_tstu_stu_cfg3_stu_cfg3,stu_cfg3,HI_BBP_TSTU_STU_CFG3_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_STU_CFG3_OFFSET)
HI_SET_GET(hi_bbp_tstu_stu_cfg4_stu_cfg4,stu_cfg4,HI_BBP_TSTU_STU_CFG4_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_STU_CFG4_OFFSET)
HI_SET_GET(hi_bbp_tstu_tds_stu_framc_tds_stu_framc,tds_stu_framc,HI_BBP_TSTU_TDS_STU_FRAMC_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TDS_STU_FRAMC_OFFSET)
HI_SET_GET(hi_bbp_tstu_tds_stu_framc_reserved,reserved,HI_BBP_TSTU_TDS_STU_FRAMC_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TDS_STU_FRAMC_OFFSET)
HI_SET_GET(hi_bbp_tstu_tds_stu_sfn_tds_stu_sfn,tds_stu_sfn,HI_BBP_TSTU_TDS_STU_SFN_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TDS_STU_SFN_OFFSET)
HI_SET_GET(hi_bbp_tstu_tds_stu_sfn_reserved,reserved,HI_BBP_TSTU_TDS_STU_SFN_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TDS_STU_SFN_OFFSET)
HI_SET_GET(hi_bbp_tstu_tds_stu_framl_tds_stu_framl,tds_stu_framl,HI_BBP_TSTU_TDS_STU_FRAML_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TDS_STU_FRAML_OFFSET)
HI_SET_GET(hi_bbp_tstu_tds_stu_framl_reserved,reserved,HI_BBP_TSTU_TDS_STU_FRAML_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TDS_STU_FRAML_OFFSET)
HI_SET_GET(hi_bbp_tstu_tds_stu_slot_tds_stu_slot,tds_stu_slot,HI_BBP_TSTU_TDS_STU_SLOT_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TDS_STU_SLOT_OFFSET)
HI_SET_GET(hi_bbp_tstu_tds_stu_slot_reserved,reserved,HI_BBP_TSTU_TDS_STU_SLOT_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_TDS_STU_SLOT_OFFSET)
HI_SET_GET(hi_bbp_tstu_clk_msr_cnt_clk_msr_cnt,clk_msr_cnt,HI_BBP_TSTU_CLK_MSR_CNT_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_CLK_MSR_CNT_OFFSET)
HI_SET_GET(hi_bbp_tstu_clk_msr_state_clk_msr_state,clk_msr_state,HI_BBP_TSTU_CLK_MSR_STATE_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_CLK_MSR_STATE_OFFSET)
HI_SET_GET(hi_bbp_tstu_clk_msr_state_reserved,reserved,HI_BBP_TSTU_CLK_MSR_STATE_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_CLK_MSR_STATE_OFFSET)
HI_SET_GET(hi_bbp_tstu_layout_cur_time_layout_cur_time,layout_cur_time,HI_BBP_TSTU_LAYOUT_CUR_TIME_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_LAYOUT_CUR_TIME_OFFSET)
HI_SET_GET(hi_bbp_tstu_layout_state_layout_state,layout_state,HI_BBP_TSTU_LAYOUT_STATE_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_LAYOUT_STATE_OFFSET)
HI_SET_GET(hi_bbp_tstu_layout_state_reserved,reserved,HI_BBP_TSTU_LAYOUT_STATE_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_LAYOUT_STATE_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_state_hifi_sfrm_int_state,hifi_sfrm_int_state,HI_BBP_TSTU_PUB_INT_STATE_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_state_arm_sfrm_int_state,arm_sfrm_int_state,HI_BBP_TSTU_PUB_INT_STATE_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_tstu_pub_int_state_reserved,reserved,HI_BBP_TSTU_PUB_INT_STATE_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_PUB_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_state_slot_int_state,slot_int_state,HI_BBP_TSTU_DSP_INT_STATE_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_state_dsp_sfrm_int_state,dsp_sfrm_int_state,HI_BBP_TSTU_DSP_INT_STATE_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_tstu_dsp_int_state_reserved,reserved,HI_BBP_TSTU_DSP_INT_STATE_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DSP_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_snf_pre_stu_dbg_sfn_pre_stu,dbg_sfn_pre_stu,HI_BBP_TSTU_DBG_SNF_PRE_STU_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_SNF_PRE_STU_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_snf_pre_stu_reserved,reserved,HI_BBP_TSTU_DBG_SNF_PRE_STU_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_SNF_PRE_STU_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_sfn_post_dbg_sfn_post_stu,dbg_sfn_post_stu,HI_BBP_TSTU_DBG_SFN_POST_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_SFN_POST_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_sfn_post_dbg_sfn_post_drx,dbg_sfn_post_drx,HI_BBP_TSTU_DBG_SFN_POST_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_SFN_POST_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_framc_pre_stu_dbg_framc_pre_stu,dbg_framc_pre_stu,HI_BBP_TSTU_DBG_FRAMC_PRE_STU_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_FRAMC_PRE_STU_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_framc_pre_stu_reserved,reserved,HI_BBP_TSTU_DBG_FRAMC_PRE_STU_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_FRAMC_PRE_STU_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_framc_post_drx_dbg_framc_post_drx,dbg_framc_post_drx,HI_BBP_TSTU_DBG_FRAMC_POST_DRX_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_FRAMC_POST_DRX_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_framc_post_drx_reserved,reserved,HI_BBP_TSTU_DBG_FRAMC_POST_DRX_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_FRAMC_POST_DRX_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_framc_post_stu_dbg_framc_post_stu,dbg_framc_post_stu,HI_BBP_TSTU_DBG_FRAMC_POST_STU_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_FRAMC_POST_STU_OFFSET)
HI_SET_GET(hi_bbp_tstu_dbg_framc_post_stu_reserved,reserved,HI_BBP_TSTU_DBG_FRAMC_POST_STU_T,HI_BBP_TSTU_BASE_ADDR, HI_BBP_TSTU_DBG_FRAMC_POST_STU_OFFSET)

#endif // __HI_BBP_TSTU_H__

