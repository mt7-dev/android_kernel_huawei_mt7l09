/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_BBP_TDSDRX.h */
/* Version       : 2.0 */
/* Created       : 2013-06-29*/
/* Last Modified : */
/* Description   :  The C union definition file for the module BBP_TDSDRX*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_BBP_TDSDRX_H__
#define __HI_BBP_TDSDRX_H__

/*
 * Project: hi
 * Module : BBP_TDSDRX
 */

#ifndef HI_SET_GET
#define HI_SET_GET(a0,a1,a2,a3,a4)
#endif

/********************************************************************************/
/*    BBP_TDSDRX 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_BBP_TDSDRX_CLK_MSR_FRAC_OFFSET                 (0x10) /* 时钟测量小数部分 */
#define    HI_BBP_TDSDRX_CLK_MSR_INTE_OFFSET                 (0x14) /* 时钟测量整数部分 */
#define    HI_BBP_TDSDRX_SFRM_FRAML_OFFSET                   (0x18) /* TDS子帧长度寄存器 */
#define    HI_BBP_TDSDRX_WAKEUP_32K_CNT_OFFSET               (0x1C) /* 睡眠唤醒时间 */
#define    HI_BBP_TDSDRX_SWITCH_32K_CNT_OFFSET               (0x20) /* 时钟切换时间 */
#define    HI_BBP_TDSDRX_SLEEP_MODE_OFFSET                   (0x24) /* 睡眠时刻控制寄存器 */
#define    HI_BBP_TDSDRX_SLEEP_START_OFFSET                  (0x28) /* 睡眠使能标志 */
#define    HI_BBP_TDSDRX_SWITCH_FORCE_START_OFFSET           (0x2C) /* 强制唤醒使能 */
#define    HI_BBP_TDSDRX_ARM_SLEEP_INT_MSK_OFFSET            (0x40) /* 中断屏蔽 */
#define    HI_BBP_TDSDRX_ARM_SLEEP_INT_CLEAR_OFFSET          (0x44) /* 中断清除 */
#define    HI_BBP_TDSDRX_ARM_SLEEP_INT_ALM_OFFSET            (0x48) /* 中断告警 */
#define    HI_BBP_TDSDRX_DRX_CFG1_OFFSET                     (0x100) /* ECO寄存器 */
#define    HI_BBP_TDSDRX_DRX_CFG2_OFFSET                     (0x104) /* ECO寄存器 */
#define    HI_BBP_TDSDRX_DRX_CFG3_OFFSET                     (0x108) /* ECO寄存器 */
#define    HI_BBP_TDSDRX_DRX_CFG4_OFFSET                     (0x10C) /* ECO寄存器 */
#define    HI_BBP_TDSDRX_CLK_SWITCH_STATE_OFFSET             (0x200) /* 时钟状态指示 */
#define    HI_BBP_TDSDRX_AWAKE_TYPE_OFFSET                   (0x204) /* 强制唤醒状态指示 */
#define    HI_BBP_TDSDRX_DRX_SFN_ACC_OFFSET                  (0x208) /* 睡眠时间内累加的子帧数 */
#define    HI_BBP_TDSDRX_SLP_TIME_CUR_OFFSET                 (0x20C) /* 当前睡眠时间 */
#define    HI_BBP_TDSDRX_ARM_INT_STATE_OFFSET                (0x220) /* 中断状态上报 */

/********************************************************************************/
/*    BBP_TDSDRX 寄存器定义（项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    clk_msr_frac               : 16; /* [15..0] 时钟测量的小数部分，小数部分为N位精度，小数部分乘以2^N。配置时刻：DSP向BBP配置睡眠使能sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_CLK_MSR_FRAC_T;    /* 时钟测量小数部分 */

typedef union
{
    struct
    {
        unsigned int    clk_msr_inte               : 12; /* [11..0] 时钟测量的整数部分。配置时刻：DSP向BBP配置睡眠使能sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_CLK_MSR_INTE_T;    /* 时钟测量整数部分 */

typedef union
{
    struct
    {
        unsigned int    sfrm_framl                 : 20; /* [19..0] TDS 5ms子帧长度，单位为cycle。默认值为5ms子帧长度值减去1，即614399配置时刻：DSP向BBP配置睡眠启动sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 12; /* [31..20] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_SFRM_FRAML_T;    /* TDS子帧长度寄存器 */

typedef union
{
    struct
    {
        unsigned int    wakeup_32k_cnt             : 27; /* [26..0] UE从配置进入睡眠到上报唤醒中断这段时间内32.768KHz时钟的计数个数，大于0，最长时间为1小时。配置时刻：DSP向BBP配置睡眠启动sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_WAKEUP_32K_CNT_T;    /* 睡眠唤醒时间 */

typedef union
{
    struct
    {
        unsigned int    switch_32k_cnt             : 12; /* [11..0] UE从上报唤醒中断到完成时钟切换这段时间内32.768KHz时钟的计数个数，大于0。在深睡时，典型配置为8ms。配置时刻：DSP向BBP配置睡眠启动sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_SWITCH_32K_CNT_T;    /* 时钟切换时间 */

typedef union
{
    struct
    {
        unsigned int    sleep_mode                 : 1; /* [0..0] 睡眠时刻控制寄存器：1'b1，任意时刻进入睡眠，配置sleep_en后逻辑立即进入睡眠1'b0，保留配置时刻：DSP向BBP配置睡眠启动sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_SLEEP_MODE_T;    /* 睡眠时刻控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    sleep_start                : 1; /* [0..0] 睡眠启动标志：1'b1，启动睡眠。配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_SLEEP_START_T;    /* 睡眠使能标志 */

typedef union
{
    struct
    {
        unsigned int    switch_force_start         : 1; /* [0..0] 强制唤醒启动：1'b1，启动强制唤醒配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_SWITCH_FORCE_START_T;    /* 强制唤醒使能 */

typedef union
{
    struct
    {
        unsigned int    switch_int_msk             : 1; /* [0..0] 时钟切换中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    wakeup_int_msk             : 1; /* [1..1] 睡眠唤醒中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_ARM_SLEEP_INT_MSK_T;    /* 中断屏蔽 */

typedef union
{
    struct
    {
        unsigned int    switch_int_clear           : 1; /* [0..0] 时钟切换中断，向该bit位写1清除对应的中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    wakeup_int_clear           : 1; /* [1..1] 睡眠唤醒中断，向该bit位写1清除对应的中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_ARM_SLEEP_INT_CLEAR_T;    /* 中断清除 */

typedef union
{
    struct
    {
        unsigned int    switch_int_alm             : 1; /* [0..0] 时钟切换中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    wakeup_int_alm             : 1; /* [1..1] 唤醒中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_ARM_SLEEP_INT_ALM_T;    /* 中断告警 */

typedef union
{
    struct
    {
        unsigned int    drx_cfg1                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_DRX_CFG1_T;    /* ECO寄存器 */

typedef union
{
    struct
    {
        unsigned int    drx_cfg2                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_DRX_CFG2_T;    /* ECO寄存器 */

typedef union
{
    struct
    {
        unsigned int    drx_cfg3                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_DRX_CFG3_T;    /* ECO寄存器 */

typedef union
{
    struct
    {
        unsigned int    drx_cfg4                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_DRX_CFG4_T;    /* ECO寄存器 */

typedef union
{
    struct
    {
        unsigned int    clk_switch_state           : 1; /* [0..0] 时钟状态标志：1'b1表示时钟切换到32.768KHz；1'b0时钟切换到系统时钟 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_CLK_SWITCH_STATE_T;    /* 时钟状态指示 */

typedef union
{
    struct
    {
        unsigned int    awake_type                 : 1; /* [0..0] 唤醒源指示：1'b1，表示此次睡眠的唤醒源是强制唤醒1'b0，表示此次睡眠是自然唤醒逻辑在启动下一次睡眠时，清楚该寄存器 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_AWAKE_TYPE_T;    /* 强制唤醒状态指示 */

typedef union
{
    struct
    {
        unsigned int    drx_sfn_acc                : 32; /* [31..0] 睡眠时间内，DRX累加的子帧数。调试用 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_DRX_SFN_ACC_T;    /* 睡眠时间内累加的子帧数 */

typedef union
{
    struct
    {
        unsigned int    slp_time_cur               : 28; /* [27..0] 当前睡眠时间上报寄存器 */
        unsigned int    reserved                   : 4; /* [31..28] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_SLP_TIME_CUR_T;    /* 当前睡眠时间 */

typedef union
{
    struct
    {
        unsigned int    switch_int_state           : 1; /* [0..0] 时钟切换中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    wakeup_int_state           : 1; /* [1..1] 睡眠唤醒中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSDRX_ARM_INT_STATE_T;    /* 中断状态上报 */


/********************************************************************************/
/*    BBP_TDSDRX 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
HI_SET_GET(hi_bbp_tdsdrx_clk_msr_frac_clk_msr_frac,clk_msr_frac,HI_BBP_TDSDRX_CLK_MSR_FRAC_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_CLK_MSR_FRAC_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_clk_msr_frac_reserved,reserved,HI_BBP_TDSDRX_CLK_MSR_FRAC_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_CLK_MSR_FRAC_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_clk_msr_inte_clk_msr_inte,clk_msr_inte,HI_BBP_TDSDRX_CLK_MSR_INTE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_CLK_MSR_INTE_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_clk_msr_inte_reserved,reserved,HI_BBP_TDSDRX_CLK_MSR_INTE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_CLK_MSR_INTE_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_sfrm_framl_sfrm_framl,sfrm_framl,HI_BBP_TDSDRX_SFRM_FRAML_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SFRM_FRAML_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_sfrm_framl_reserved,reserved,HI_BBP_TDSDRX_SFRM_FRAML_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SFRM_FRAML_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_wakeup_32k_cnt_wakeup_32k_cnt,wakeup_32k_cnt,HI_BBP_TDSDRX_WAKEUP_32K_CNT_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_WAKEUP_32K_CNT_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_wakeup_32k_cnt_reserved,reserved,HI_BBP_TDSDRX_WAKEUP_32K_CNT_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_WAKEUP_32K_CNT_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_switch_32k_cnt_switch_32k_cnt,switch_32k_cnt,HI_BBP_TDSDRX_SWITCH_32K_CNT_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SWITCH_32K_CNT_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_switch_32k_cnt_reserved,reserved,HI_BBP_TDSDRX_SWITCH_32K_CNT_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SWITCH_32K_CNT_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_sleep_mode_sleep_mode,sleep_mode,HI_BBP_TDSDRX_SLEEP_MODE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SLEEP_MODE_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_sleep_mode_reserved,reserved,HI_BBP_TDSDRX_SLEEP_MODE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SLEEP_MODE_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_sleep_start_sleep_start,sleep_start,HI_BBP_TDSDRX_SLEEP_START_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SLEEP_START_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_sleep_start_reserved,reserved,HI_BBP_TDSDRX_SLEEP_START_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SLEEP_START_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_switch_force_start_switch_force_start,switch_force_start,HI_BBP_TDSDRX_SWITCH_FORCE_START_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SWITCH_FORCE_START_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_switch_force_start_reserved,reserved,HI_BBP_TDSDRX_SWITCH_FORCE_START_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SWITCH_FORCE_START_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_sleep_int_msk_switch_int_msk,switch_int_msk,HI_BBP_TDSDRX_ARM_SLEEP_INT_MSK_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_SLEEP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_sleep_int_msk_wakeup_int_msk,wakeup_int_msk,HI_BBP_TDSDRX_ARM_SLEEP_INT_MSK_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_SLEEP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_sleep_int_msk_reserved,reserved,HI_BBP_TDSDRX_ARM_SLEEP_INT_MSK_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_SLEEP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_sleep_int_clear_switch_int_clear,switch_int_clear,HI_BBP_TDSDRX_ARM_SLEEP_INT_CLEAR_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_SLEEP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_sleep_int_clear_wakeup_int_clear,wakeup_int_clear,HI_BBP_TDSDRX_ARM_SLEEP_INT_CLEAR_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_SLEEP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_sleep_int_clear_reserved,reserved,HI_BBP_TDSDRX_ARM_SLEEP_INT_CLEAR_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_SLEEP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_sleep_int_alm_switch_int_alm,switch_int_alm,HI_BBP_TDSDRX_ARM_SLEEP_INT_ALM_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_SLEEP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_sleep_int_alm_wakeup_int_alm,wakeup_int_alm,HI_BBP_TDSDRX_ARM_SLEEP_INT_ALM_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_SLEEP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_sleep_int_alm_reserved,reserved,HI_BBP_TDSDRX_ARM_SLEEP_INT_ALM_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_SLEEP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_drx_cfg1_drx_cfg1,drx_cfg1,HI_BBP_TDSDRX_DRX_CFG1_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_DRX_CFG1_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_drx_cfg2_drx_cfg2,drx_cfg2,HI_BBP_TDSDRX_DRX_CFG2_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_DRX_CFG2_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_drx_cfg3_drx_cfg3,drx_cfg3,HI_BBP_TDSDRX_DRX_CFG3_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_DRX_CFG3_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_drx_cfg4_drx_cfg4,drx_cfg4,HI_BBP_TDSDRX_DRX_CFG4_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_DRX_CFG4_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_clk_switch_state_clk_switch_state,clk_switch_state,HI_BBP_TDSDRX_CLK_SWITCH_STATE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_CLK_SWITCH_STATE_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_clk_switch_state_reserved,reserved,HI_BBP_TDSDRX_CLK_SWITCH_STATE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_CLK_SWITCH_STATE_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_awake_type_awake_type,awake_type,HI_BBP_TDSDRX_AWAKE_TYPE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_AWAKE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_awake_type_reserved,reserved,HI_BBP_TDSDRX_AWAKE_TYPE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_AWAKE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_drx_sfn_acc_drx_sfn_acc,drx_sfn_acc,HI_BBP_TDSDRX_DRX_SFN_ACC_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_DRX_SFN_ACC_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_slp_time_cur_slp_time_cur,slp_time_cur,HI_BBP_TDSDRX_SLP_TIME_CUR_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SLP_TIME_CUR_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_slp_time_cur_reserved,reserved,HI_BBP_TDSDRX_SLP_TIME_CUR_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_SLP_TIME_CUR_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_int_state_switch_int_state,switch_int_state,HI_BBP_TDSDRX_ARM_INT_STATE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_int_state_wakeup_int_state,wakeup_int_state,HI_BBP_TDSDRX_ARM_INT_STATE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_tdsdrx_arm_int_state_reserved,reserved,HI_BBP_TDSDRX_ARM_INT_STATE_T,HI_BBP_TDSDRX_BASE_ADDR, HI_BBP_TDSDRX_ARM_INT_STATE_OFFSET)

#endif // __HI_BBP_TDSDRX_H__

