/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_bbp_ltedrx.h */
/* Version       : 2.0 */
/* Author        : xxx*/
/* Created       : 2013-06-06*/
/* Last Modified : */
/* Description   :  The C union definition file for the module bbp_ltedrx*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Author        : xxx*/
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_BBP_LTEDRX_H__
#define __HI_BBP_LTEDRX_H__

/*
 * Project: hi
 * Module : bbp_ltedrx
 */

#ifndef HI_SET_GET
#define HI_SET_GET(a0,a1,a2,a3,a4)
#endif

/********************************************************************************/
/*    bbp_ltedrx 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_BBP_LTEDRX_CLK_MSR_FRAC_OFFSET                 (0x10) /* 时钟测量小数部分 */
#define    HI_BBP_LTEDRX_CLK_MSR_INTE_OFFSET                 (0x14) /* 时钟测量整数部分 */
#define    HI_BBP_LTEDRX_SFRM_FRAML_OFFSET                   (0x18) /* LTE子帧长度寄存器 */
#define    HI_BBP_LTEDRX_WAKEUP_32K_CNT_OFFSET               (0x1C) /* 睡眠唤醒时间 */
#define    HI_BBP_LTEDRX_SWITCH_32K_CNT_OFFSET               (0x20) /* 时钟切换时间 */
#define    HI_BBP_LTEDRX_SLEEP_MODE_OFFSET                   (0x24) /* 睡眠时刻控制寄存器 */
#define    HI_BBP_LTEDRX_SLEEP_START_OFFSET                  (0x28) /* 睡眠使能标志 */
#define    HI_BBP_LTEDRX_SWITCH_FORCE_START_OFFSET           (0x2C) /* 强制唤醒使能 */
#define    HI_BBP_LTEDRX_ARM_SLEEP_INT_MSK_OFFSET            (0x40) /* 中断屏蔽 */
#define    HI_BBP_LTEDRX_ARM_SLEEP_INT_CLEAR_OFFSET          (0x44) /* 中断清除 */
#define    HI_BBP_LTEDRX_ARM_SLEEP_INT_ALM_OFFSET            (0x48) /* 中断告警 */
#define    HI_BBP_LTEDRX_LTE_SYS_TIMER_OFFSET                (0x70) /* 系统更新定时器 */
#define    HI_BBP_LTEDRX_LTE_3412_TIMER_OFFSET               (0x74) /* TAU更新定时器 */
#define    HI_BBP_LTEDRX_LTE_VPLMN_TIMER_OFFSET              (0x78) /* VPLMN定时器 */
#define    HI_BBP_LTEDRX_LTE_TSRHC_TIMER_OFFSET              (0x7C) /* 搜网定时器 */
#define    HI_BBP_LTEDRX_LTE_TA_TIMER_OFFSET                 (0x80) /* TA调整定时器 */
#define    HI_BBP_LTEDRX_LTE_DEEPS_TIMER_OFFSET              (0x84) /* 深睡眠定时器 */
#define    HI_BBP_LTEDRX_LTE_PS32K_TIMER_OFFSET              (0x88) /* 协议找定时器超时计数器 */
#define    HI_BBP_LTEDRX_LTE_PS32KBASE_TIMER_OFFSET          (0x8C) /* 协议找定时器基准计数器 */
#define    HI_BBP_LTEDRX_LTE_PS32K_BASE_MODE_OFFSET          (0x90) /* 协议找定时器基准计数器工作模式 */
#define    HI_BBP_LTEDRX_DRX_TIMER_EN_OFFSET                 (0x94) /* DRX tim定时器使能 */
#define    HI_BBP_LTEDRX_DRX_TIMER_10MS_OFFSET               (0x98) /* 10M定时长度 */
#define    HI_BBP_LTEDRX_DRX_TIMER_MIN_OFFSET                (0x9C) /* 1分钟定时长度 */
#define    HI_BBP_LTEDRX_DRX_TIMER_HOUR_OFFSET               (0xA0) /* 1小时定时长度 */
#define    HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_OFFSET            (0xB0) /* 中断屏蔽 */
#define    HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_OFFSET          (0xB4) /* 中断清除 */
#define    HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_OFFSET            (0xB8) /* 中断告警 */
#define    HI_BBP_LTEDRX_CLK_SWITCH_STATE_OFFSET             (0x200) /* 时钟状态指示 */
#define    HI_BBP_LTEDRX_AWAKE_TYPE_OFFSET                   (0x204) /* 唤醒状态指示 */
#define    HI_BBP_LTEDRX_DRX_SFN_ACC_OFFSET                  (0x208) /* 睡眠时间内累加的子帧数 */
#define    HI_BBP_LTEDRX_SLP_TIME_CUR_OFFSET                 (0x20C) /* 当前睡眠时间 */
#define    HI_BBP_LTEDRX_ARM_INT_STATE_OFFSET                (0x220) /* 中断状态上报 */
#define    HI_BBP_LTEDRX_LTE_SYS_CUR_TIMER_OFFSET            (0x300) /* 系统更新定时器 */
#define    HI_BBP_LTEDRX_LTE_3412_CUR_TIMER_OFFSET           (0x304) /* TAU更新定时器 */
#define    HI_BBP_LTEDRX_LTE_VPLMN_CUR_TIMER_OFFSET          (0x308) /* VPLMN定时器 */
#define    HI_BBP_LTEDRX_LTE_TSRHC_CUR_TIMER_OFFSET          (0x30C) /* 搜网定时器 */
#define    HI_BBP_LTEDRX_LTE_TA_CUR_TIMER_OFFSET             (0x310) /* TA调整定时器 */
#define    HI_BBP_LTEDRX_LTE_DEEPS_CUR_TIMER_OFFSET          (0x314) /* 深睡眠定时器 */
#define    HI_BBP_LTEDRX_LTE_PS32K_CUR_TIMER_OFFSET          (0x318) /* 协议找定时器超时计数器 */
#define    HI_BBP_LTEDRX_LTE_PS32KBASE_CUR_TIMER_OFFSET      (0x31C) /* 协议找定时器基准计数器 */
#define    HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_OFFSET          (0x340) /* 中断状态上报 */


#ifndef __ASSEMBLY__

/********************************************************************************/
/*    bbp_ltedrx 寄存器定义（项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    clk_msr_frac               : 16; /* [15..0] 时钟测量的小数部分，小数部分为N位精度，小数部分乘以2^N。配置时刻：DSP向BBP配置睡眠使能sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_CLK_MSR_FRAC_T;    /* 时钟测量小数部分 */

typedef union
{
    struct
    {
        unsigned int    clk_msr_inte               : 12; /* [11..0] 时钟测量的整数部分。配置时刻：DSP向BBP配置睡眠使能sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_CLK_MSR_INTE_T;    /* 时钟测量整数部分 */

typedef union
{
    struct
    {
        unsigned int    sfrm_framl                 : 20; /* [19..0] LTE 1ms子帧长度，单位为cycle。默认值为1ms子帧长度值减去1，即122879配置时刻：DSP向BBP配置睡眠启动sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 12; /* [31..20] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_SFRM_FRAML_T;    /* LTE子帧长度寄存器 */

typedef union
{
    struct
    {
        unsigned int    wakeup_32k_cnt             : 27; /* [26..0] UE从配置进入睡眠到上报唤醒中断这段时间内32.768KHz时钟的计数个数，大于0，最长时间为1小时。配置时刻：DSP向BBP配置睡眠启动sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_WAKEUP_32K_CNT_T;    /* 睡眠唤醒时间 */

typedef union
{
    struct
    {
        unsigned int    switch_32k_cnt             : 12; /* [11..0] UE从上报唤醒中断到完成时钟切换这段时间内32.768KHz时钟的计数个数，大于0。在深睡时，典型配置为8ms。配置时刻：DSP向BBP配置睡眠启动sleep_en前配置生效时刻：立即 */
        unsigned int    reserved                   : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_SWITCH_32K_CNT_T;    /* 时钟切换时间 */

typedef union
{
    struct
    {
        unsigned int    sleep_mode                 : 1; /* [0..0] 睡眠时刻控制寄存器：1'b1，任意时刻进入睡眠，配置sleep_start后逻辑立即进入睡眠1'b0，保留配置时刻：DSP向BBP配置睡眠启动sleep_start前配置生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_SLEEP_MODE_T;    /* 睡眠时刻控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    sleep_start                : 1; /* [0..0] 睡眠启动标志：1'b1，启动睡眠。配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_SLEEP_START_T;    /* 睡眠使能标志 */

typedef union
{
    struct
    {
        unsigned int    switch_force_start         : 1; /* [0..0] 强制唤醒启动：1'b1，启动强制唤醒配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_SWITCH_FORCE_START_T;    /* 强制唤醒使能 */

typedef union
{
    struct
    {
        unsigned int    switch_int_msk             : 1; /* [0..0] 时钟切换中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    wakeup_int_msk             : 1; /* [1..1] 睡眠唤醒中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_ARM_SLEEP_INT_MSK_T;    /* 中断屏蔽 */

typedef union
{
    struct
    {
        unsigned int    switch_int_clear           : 1; /* [0..0] 时钟切换中断，向该bit位写1清除对应的中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    wakeup_int_clear           : 1; /* [1..1] 睡眠唤醒中断，向该bit位写1清除对应的中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_ARM_SLEEP_INT_CLEAR_T;    /* 中断清除 */

typedef union
{
    struct
    {
        unsigned int    switch_int_alm             : 1; /* [0..0] 时钟切换中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    wakeup_int_alm             : 1; /* [1..1] 唤醒中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_ARM_SLEEP_INT_ALM_T;    /* 中断告警 */

typedef union
{
    struct
    {
        unsigned int    lte_sys_timer              : 3; /* [2..0] 系统更新定时器，定时周期 3 hour。若BBP处于睡眠时，计数器溢出，则等到BBP被唤醒后再上报定时中断。配置时刻：启动该定时器之前生效时刻：立即 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_SYS_TIMER_T;    /* 系统更新定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_3412_timer             : 8; /* [7..0] TAU更新定时器，定时周期 0~186 min。若BBP处于睡眠时，计数器溢出，则等到BBP被唤醒后再上报定时中断。配置时刻：启动该定时器之前生效时刻：立即 */
        unsigned int    reserved                   : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_3412_TIMER_T;    /* TAU更新定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_vplmn_timer            : 3; /* [2..0] VPLMN状态下搜索高优先级PLMN定时器，定时周期 2 min。若BBP处于睡眠时，计数器溢出，则等到BBP被唤醒后再上报定时中断。配置时刻：启动该定时器之前生效时刻：立即 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_VPLMN_TIMER_T;    /* VPLMN定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_tsrhc_timer            : 3; /* [2..0] 出服务区搜网定时器，定时周期 3 min。若BBP处于睡眠时，计数器溢出，则等到BBP被唤醒后再上报定时中断。配置时刻：启动该定时器之前生效时刻：立即 */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_TSRHC_TIMER_T;    /* 搜网定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_ta_timer               : 11; /* [10..0] TA调整定时器，定时周期取值为： 500ms、750ms、1280ms、1920ms、2560ms、5120ms、10240ms、infinity。若BBP处于睡眠时，计数器溢出，则等到BBP被唤醒后再上报定时中断。配置时刻：启动该定时器之前生效时刻：立即 */
        unsigned int    reserved                   : 21; /* [31..11] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_TA_TIMER_T;    /* TA调整定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_deeps_timer            : 7; /* [6..0] 深睡眠定时器，定时周期单位为min。配置时刻：启动该定时器之前生效时刻：立即 */
        unsigned int    reserved                   : 25; /* [31..7] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_DEEPS_TIMER_T;    /* 深睡眠定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_ps32k_timer            : 32; /* [31..0] 协议栈定时器超时计数器。采用32.768KHz睡眠时钟计数，单位为一个32KHz时钟周期，用以驱动DRX睡眠中仍然需要运行的软件定时器。定时长度由软件进行配置，超过定时长度则产生中断并上报给ARM。若BBP处于睡眠时，计数器溢出，则等到BBP被唤醒后再上报定时中断。配置时刻：启动该定时器之前生效时刻：立即 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_PS32K_TIMER_T;    /* 协议找定时器超时计数器 */

typedef union
{
    struct
    {
        unsigned int    lte_ps32kbase_timer        : 32; /* [31..0] 协议栈定时器基准计数器，用以补偿协议栈定时器在睡眠期间的计数值。采用32.768KHz睡眠时钟计数，单位为一个32KHz时钟周期，该计数器支持两种模式：（1）用户定义模式。定时长度由软件配置，计数器值超过定时长度则产生中断并上报给ARM。若BBP处于睡眠时，计数器溢出，则等到BBP被唤醒后再上报定时中断。（2）Free模式。上电后，计数器初值为0xFFFFFFFF，递减计数，计数到0后自动循环，再重新计数。配置时刻：启动该定时器之前生效时刻：立即 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_PS32KBASE_TIMER_T;    /* 协议找定时器基准计数器 */

typedef union
{
    struct
    {
        unsigned int    lte_ps32k_base_mode        : 1; /* [0..0] 协议找定时器基准计数器工作模式选择：1'b0，Free模式1'b1，用户定义模式配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_PS32K_BASE_MODE_T;    /* 协议找定时器基准计数器工作模式 */

typedef union
{
    struct
    {
        unsigned int    lte_sys_timer_en           : 1; /* [0..0] SYS定时器启动信号：1'b1，启动定时器每一次定时结束后，软件再次启动定时器时需要将定时器使能配置为0，然后再配置为1配置时刻：任意生效时刻：立即 */
        unsigned int    lte_3412_timer_en          : 1; /* [1..1] 3412定时器启动信号：1'b1，启动定时器每一次定时结束后，软件再次启动定时器时需要将定时器使能配置为0，然后再配置为1配置时刻：任意生效时刻：立即 */
        unsigned int    lte_vplmn_timer_en         : 1; /* [2..2] VPLMN定时器启动信号：1'b1，启动定时器每一次定时结束后，软件再次启动定时器时需要将定时器使能配置为0，然后再配置为1配置时刻：任意生效时刻：立即 */
        unsigned int    lte_tsrhc_timer_en         : 1; /* [3..3] TSRHC定时器启动信号：1'b1，启动定时器每一次定时结束后，软件再次启动定时器时需要将定时器使能配置为0，然后再配置为1配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ta_timer_en            : 1; /* [4..4] TA定时器启动信号：1'b1，启动定时器每一次定时结束后，软件再次启动定时器时需要将定时器使能配置为0，然后再配置为1配置时刻：任意生效时刻：立即 */
        unsigned int    lte_deeps_timer_en         : 1; /* [5..5] 深睡眠定时器启动信号：1'b1，启动定时器每一次定时结束后，软件再次启动定时器时需要将定时器使能配置为0，然后再配置为1配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ps32k_timer_en         : 1; /* [6..6] 协议栈定时器超时计数器启动信号：1'b1，启动定时器每一次定时结束后，软件再次启动定时器时需要将定时器使能配置为0，然后再配置为1配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ps32kbase_timer_en     : 1; /* [7..7] 协议栈定时器基准计数器启动信号：1'b1，启动定时器每一次定时结束后，软件再次启动定时器时需要将定时器使能配置为0，然后再配置为1配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_DRX_TIMER_EN_T;    /* DRX tim定时器使能 */

typedef union
{
    struct
    {
        unsigned int    drx_timer_10ms             : 9; /* [8..0] 10ms中32K时钟个数，调试用 */
        unsigned int    reserved                   : 23; /* [31..9] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_DRX_TIMER_10MS_T;    /* 10M定时长度 */

typedef union
{
    struct
    {
        unsigned int    drx_timer_min              : 21; /* [20..0] 1min中32K时钟个数，调试用 */
        unsigned int    reserved                   : 11; /* [31..21] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_DRX_TIMER_MIN_T;    /* 1分钟定时长度 */

typedef union
{
    struct
    {
        unsigned int    drx_timer_hour             : 6; /* [5..0] 1hour中的分钟数，默认为60min，调试用 */
        unsigned int    reserved                   : 26; /* [31..6] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_DRX_TIMER_HOUR_T;    /* 1小时定时长度 */

typedef union
{
    struct
    {
        unsigned int    lte_sys_int_msk            : 1; /* [0..0] SYS定时器中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    lte_3412_int_msk           : 1; /* [1..1] TAU定时器中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    lte_vplmn_int_msk          : 1; /* [2..2] VPLMN定时器中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    lte_tsrhc_int_msk          : 1; /* [3..3] TSRHC定时器中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ta_int_msk             : 1; /* [4..4] TA定时器中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    lte_deeps_int_msk          : 1; /* [5..5] 深睡眠定时器中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ps32k_int_msk          : 1; /* [6..6] 协议栈定时器超时计数器中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ps32kbase_int_msk      : 1; /* [7..7] 协议栈定时器基准计数器中断屏蔽：1'b1，打开中断1'b0，屏蔽中断配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_T;    /* 中断屏蔽 */

typedef union
{
    struct
    {
        unsigned int    lte_sys_int_clear          : 1; /* [0..0] SYS定时器中断清除，向该bit位写1清除对应的定时器中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_3412_int_clear         : 1; /* [1..1] TAU定时器中断清除，向该bit位写1清除对应的定时器中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_vplmn_int_clear        : 1; /* [2..2] VPLMN定时器中断清除，向该bit位写1清除对应的定时器中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_tsrhc_int_clear        : 1; /* [3..3] TSRHC定时器中断清除，向该bit位写1清除对应的定时器中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ta_int_clear           : 1; /* [4..4] TA定时器中断清除，向该bit位写1清除对应的定时器中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_deeps_int_clear        : 1; /* [5..5] 深睡眠定时器中断清除，向该bit位写1清除对应的定时器中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ps32k_int_clear        : 1; /* [6..6] 协议栈定时器超时计数器中断清除，向该bit位写1清除对应的定时器中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ps32kbase_int_clear    : 1; /* [7..7] 协议栈定时器基准计数器中断清除，向该bit位写1清除对应的定时器中断及状态。配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_T;    /* 中断清除 */

typedef union
{
    struct
    {
        unsigned int    lte_sys_int_alm            : 1; /* [0..0] SYS定时器中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_3412_int_alm           : 1; /* [1..1] TAU定时器中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_vplmn_int_alm          : 1; /* [2..2] VPLMN定时器中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_tsrhc_int_alm          : 1; /* [3..3] TSRHC定时器中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ta_int_alm             : 1; /* [4..4] TA定时器中断告警。当新中断触发而原中断没被清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_deeps_int_alm          : 1; /* [5..5] 深睡眠定时器中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ps32k_int_alm          : 1; /* [6..6] 协议栈定时器超时计数器中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    lte_ps32kbase_int_alm      : 1; /* [7..7] 协议栈定时器基准计数器中断告警。当新中断触发而原中断未清除时产生告警，写1清0告警位。配置时刻：任意生效时刻：立即 */
        unsigned int    reserved                   : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_T;    /* 中断告警 */

typedef union
{
    struct
    {
        unsigned int    clk_switch_state           : 1; /* [0..0] 时钟状态标志：1'b1表示时钟切换到32.768KHz；1'b0时钟切换到系统时钟 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_CLK_SWITCH_STATE_T;    /* 时钟状态指示 */

typedef union
{
    struct
    {
        unsigned int    awake_type                 : 1; /* [0..0] 唤醒源指示：1'b1，表示此次睡眠的唤醒源是强制唤醒1'b0，表示此次睡眠是自然唤醒逻辑在启动下一次睡眠时，清楚该寄存器 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_AWAKE_TYPE_T;    /* 唤醒状态指示 */

typedef union
{
    struct
    {
        unsigned int    drx_sfn_acc                : 32; /* [31..0] 睡眠时间内，DRX累加的子帧数。调试用 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_DRX_SFN_ACC_T;    /* 睡眠时间内累加的子帧数 */

typedef union
{
    struct
    {
        unsigned int    slp_time_cur               : 28; /* [27..0] 当前睡眠时间上报寄存器 */
        unsigned int    reserved                   : 4; /* [31..28] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_SLP_TIME_CUR_T;    /* 当前睡眠时间 */

typedef union
{
    struct
    {
        unsigned int    switch_int_state           : 1; /* [0..0] 时钟切换中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    wakeup_int_state           : 1; /* [1..1] 睡眠唤醒中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_ARM_INT_STATE_T;    /* 中断状态上报 */

typedef union
{
    struct
    {
        unsigned int    lte_sys_cur_timer          : 3; /* [2..0] 系统更新定时器当前值，若该定时器未启动，默认值为0x0，单位为hour */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_SYS_CUR_TIMER_T;    /* 系统更新定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_3412_cur_timer         : 8; /* [7..0] TAU更新定时器当前值，若该定时器未启动，默认值为0x0，单位为min */
        unsigned int    reserved                   : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_3412_CUR_TIMER_T;    /* TAU更新定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_vplmn_cur_timer        : 3; /* [2..0] VPLMN状态下搜索高优先级PLMN定时器当前值，若该定时器未启动，默认值为0x0，单位为min */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_VPLMN_CUR_TIMER_T;    /* VPLMN定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_tsrhc_cur_timer        : 3; /* [2..0] 出服务区搜网定时器当前值，若该定时器未启动，默认值为0x0，单位为min */
        unsigned int    reserved                   : 29; /* [31..3] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_TSRHC_CUR_TIMER_T;    /* 搜网定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_ta_cur_timer           : 11; /* [10..0] TA调整定时器当前值，若该定时器未启动，默认值为0x0，单位为ms */
        unsigned int    reserved                   : 21; /* [31..11] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_TA_CUR_TIMER_T;    /* TA调整定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_deeps_cur_timer        : 7; /* [6..0] 深睡眠定时器当前值，若该定时器未启动，默认值为0x0，单位为min */
        unsigned int    reserved                   : 25; /* [31..7] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_DEEPS_CUR_TIMER_T;    /* 深睡眠定时器 */

typedef union
{
    struct
    {
        unsigned int    lte_ps32k_cur_timer        : 32; /* [31..0] 协议栈定时器超时计数器当前值，若该定时器未启动，默认值为0x0，单位为一个32.768KHz时钟 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_PS32K_CUR_TIMER_T;    /* 协议找定时器超时计数器 */

typedef union
{
    struct
    {
        unsigned int    lte_ps32kbase_cur_timer    : 32; /* [31..0] 协议栈定时器基准计数器当前值，若该定时器未启动，默认值为0x0，单位为一个32.768KHz时钟 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_LTE_PS32KBASE_CUR_TIMER_T;    /* 协议找定时器基准计数器 */

typedef union
{
    struct
    {
        unsigned int    lte_sys_int_state          : 1; /* [0..0] SYS定时器中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    lte_3412_int_state         : 1; /* [1..1] TAU定时器中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    lte_vplmn_int_state        : 1; /* [2..2] VPLMN定时器中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    lte_tsrhc_int_state        : 1; /* [3..3] TSRHC定时器中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    lte_ta_int_state           : 1; /* [4..4] TA定时器中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    lte_deeps_int_state        : 1; /* [5..5] 深睡眠定时器中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    lte_ps32k_int_state        : 1; /* [6..6] 协议栈定时器超时计数器中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    lte_ps32kbase_int_state    : 1; /* [7..7] 协议栈定时器基准计数器中断状态指示。向中断清除寄存器ARM_INT_CLEAR对应位写1'b1可清除中断标志 */
        unsigned int    reserved                   : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_T;    /* 中断状态上报 */


/********************************************************************************/
/*    bbp_ltedrx 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
HI_SET_GET(hi_bbp_ltedrx_clk_msr_frac_clk_msr_frac,clk_msr_frac,HI_BBP_LTEDRX_CLK_MSR_FRAC_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_CLK_MSR_FRAC_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_clk_msr_frac_reserved,reserved,HI_BBP_LTEDRX_CLK_MSR_FRAC_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_CLK_MSR_FRAC_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_clk_msr_inte_clk_msr_inte,clk_msr_inte,HI_BBP_LTEDRX_CLK_MSR_INTE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_CLK_MSR_INTE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_clk_msr_inte_reserved,reserved,HI_BBP_LTEDRX_CLK_MSR_INTE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_CLK_MSR_INTE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_sfrm_framl_sfrm_framl,sfrm_framl,HI_BBP_LTEDRX_SFRM_FRAML_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SFRM_FRAML_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_sfrm_framl_reserved,reserved,HI_BBP_LTEDRX_SFRM_FRAML_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SFRM_FRAML_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_wakeup_32k_cnt_wakeup_32k_cnt,wakeup_32k_cnt,HI_BBP_LTEDRX_WAKEUP_32K_CNT_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_WAKEUP_32K_CNT_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_wakeup_32k_cnt_reserved,reserved,HI_BBP_LTEDRX_WAKEUP_32K_CNT_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_WAKEUP_32K_CNT_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_switch_32k_cnt_switch_32k_cnt,switch_32k_cnt,HI_BBP_LTEDRX_SWITCH_32K_CNT_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SWITCH_32K_CNT_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_switch_32k_cnt_reserved,reserved,HI_BBP_LTEDRX_SWITCH_32K_CNT_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SWITCH_32K_CNT_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_sleep_mode_sleep_mode,sleep_mode,HI_BBP_LTEDRX_SLEEP_MODE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SLEEP_MODE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_sleep_mode_reserved,reserved,HI_BBP_LTEDRX_SLEEP_MODE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SLEEP_MODE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_sleep_start_sleep_start,sleep_start,HI_BBP_LTEDRX_SLEEP_START_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SLEEP_START_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_sleep_start_reserved,reserved,HI_BBP_LTEDRX_SLEEP_START_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SLEEP_START_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_switch_force_start_switch_force_start,switch_force_start,HI_BBP_LTEDRX_SWITCH_FORCE_START_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SWITCH_FORCE_START_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_switch_force_start_reserved,reserved,HI_BBP_LTEDRX_SWITCH_FORCE_START_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SWITCH_FORCE_START_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_sleep_int_msk_switch_int_msk,switch_int_msk,HI_BBP_LTEDRX_ARM_SLEEP_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_SLEEP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_sleep_int_msk_wakeup_int_msk,wakeup_int_msk,HI_BBP_LTEDRX_ARM_SLEEP_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_SLEEP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_sleep_int_msk_reserved,reserved,HI_BBP_LTEDRX_ARM_SLEEP_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_SLEEP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_sleep_int_clear_switch_int_clear,switch_int_clear,HI_BBP_LTEDRX_ARM_SLEEP_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_SLEEP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_sleep_int_clear_wakeup_int_clear,wakeup_int_clear,HI_BBP_LTEDRX_ARM_SLEEP_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_SLEEP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_sleep_int_clear_reserved,reserved,HI_BBP_LTEDRX_ARM_SLEEP_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_SLEEP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_sleep_int_alm_switch_int_alm,switch_int_alm,HI_BBP_LTEDRX_ARM_SLEEP_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_SLEEP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_sleep_int_alm_wakeup_int_alm,wakeup_int_alm,HI_BBP_LTEDRX_ARM_SLEEP_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_SLEEP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_sleep_int_alm_reserved,reserved,HI_BBP_LTEDRX_ARM_SLEEP_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_SLEEP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_sys_timer_lte_sys_timer,lte_sys_timer,HI_BBP_LTEDRX_LTE_SYS_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_SYS_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_sys_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_SYS_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_SYS_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_3412_timer_lte_3412_timer,lte_3412_timer,HI_BBP_LTEDRX_LTE_3412_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_3412_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_3412_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_3412_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_3412_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_vplmn_timer_lte_vplmn_timer,lte_vplmn_timer,HI_BBP_LTEDRX_LTE_VPLMN_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_VPLMN_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_vplmn_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_VPLMN_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_VPLMN_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_tsrhc_timer_lte_tsrhc_timer,lte_tsrhc_timer,HI_BBP_LTEDRX_LTE_TSRHC_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_TSRHC_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_tsrhc_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_TSRHC_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_TSRHC_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_ta_timer_lte_ta_timer,lte_ta_timer,HI_BBP_LTEDRX_LTE_TA_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_TA_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_ta_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_TA_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_TA_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_deeps_timer_lte_deeps_timer,lte_deeps_timer,HI_BBP_LTEDRX_LTE_DEEPS_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_DEEPS_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_deeps_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_DEEPS_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_DEEPS_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_ps32k_timer_lte_ps32k_timer,lte_ps32k_timer,HI_BBP_LTEDRX_LTE_PS32K_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_PS32K_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_ps32kbase_timer_lte_ps32kbase_timer,lte_ps32kbase_timer,HI_BBP_LTEDRX_LTE_PS32KBASE_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_PS32KBASE_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_ps32k_base_mode_lte_ps32k_base_mode,lte_ps32k_base_mode,HI_BBP_LTEDRX_LTE_PS32K_BASE_MODE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_PS32K_BASE_MODE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_ps32k_base_mode_reserved,reserved,HI_BBP_LTEDRX_LTE_PS32K_BASE_MODE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_PS32K_BASE_MODE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_en_lte_sys_timer_en,lte_sys_timer_en,HI_BBP_LTEDRX_DRX_TIMER_EN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_EN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_en_lte_3412_timer_en,lte_3412_timer_en,HI_BBP_LTEDRX_DRX_TIMER_EN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_EN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_en_lte_vplmn_timer_en,lte_vplmn_timer_en,HI_BBP_LTEDRX_DRX_TIMER_EN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_EN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_en_lte_tsrhc_timer_en,lte_tsrhc_timer_en,HI_BBP_LTEDRX_DRX_TIMER_EN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_EN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_en_lte_ta_timer_en,lte_ta_timer_en,HI_BBP_LTEDRX_DRX_TIMER_EN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_EN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_en_lte_deeps_timer_en,lte_deeps_timer_en,HI_BBP_LTEDRX_DRX_TIMER_EN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_EN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_en_lte_ps32k_timer_en,lte_ps32k_timer_en,HI_BBP_LTEDRX_DRX_TIMER_EN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_EN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_en_lte_ps32kbase_timer_en,lte_ps32kbase_timer_en,HI_BBP_LTEDRX_DRX_TIMER_EN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_EN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_en_reserved,reserved,HI_BBP_LTEDRX_DRX_TIMER_EN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_EN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_10ms_drx_timer_10ms,drx_timer_10ms,HI_BBP_LTEDRX_DRX_TIMER_10MS_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_10MS_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_10ms_reserved,reserved,HI_BBP_LTEDRX_DRX_TIMER_10MS_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_10MS_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_min_drx_timer_min,drx_timer_min,HI_BBP_LTEDRX_DRX_TIMER_MIN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_MIN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_min_reserved,reserved,HI_BBP_LTEDRX_DRX_TIMER_MIN_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_MIN_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_hour_drx_timer_hour,drx_timer_hour,HI_BBP_LTEDRX_DRX_TIMER_HOUR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_HOUR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_timer_hour_reserved,reserved,HI_BBP_LTEDRX_DRX_TIMER_HOUR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_TIMER_HOUR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_msk_lte_sys_int_msk,lte_sys_int_msk,HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_msk_lte_3412_int_msk,lte_3412_int_msk,HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_msk_lte_vplmn_int_msk,lte_vplmn_int_msk,HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_msk_lte_tsrhc_int_msk,lte_tsrhc_int_msk,HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_msk_lte_ta_int_msk,lte_ta_int_msk,HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_msk_lte_deeps_int_msk,lte_deeps_int_msk,HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_msk_lte_ps32k_int_msk,lte_ps32k_int_msk,HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_msk_lte_ps32kbase_int_msk,lte_ps32kbase_int_msk,HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_msk_reserved,reserved,HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_clear_lte_sys_int_clear,lte_sys_int_clear,HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_clear_lte_3412_int_clear,lte_3412_int_clear,HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_clear_lte_vplmn_int_clear,lte_vplmn_int_clear,HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_clear_lte_tsrhc_int_clear,lte_tsrhc_int_clear,HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_clear_lte_ta_int_clear,lte_ta_int_clear,HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_clear_lte_deeps_int_clear,lte_deeps_int_clear,HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_clear_lte_ps32k_int_clear,lte_ps32k_int_clear,HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_clear_lte_ps32kbase_int_clear,lte_ps32kbase_int_clear,HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_clear_reserved,reserved,HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_alm_lte_sys_int_alm,lte_sys_int_alm,HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_alm_lte_3412_int_alm,lte_3412_int_alm,HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_alm_lte_vplmn_int_alm,lte_vplmn_int_alm,HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_alm_lte_tsrhc_int_alm,lte_tsrhc_int_alm,HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_alm_lte_ta_int_alm,lte_ta_int_alm,HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_alm_lte_deeps_int_alm,lte_deeps_int_alm,HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_alm_lte_ps32k_int_alm,lte_ps32k_int_alm,HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_alm_lte_ps32kbase_int_alm,lte_ps32kbase_int_alm,HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_alm_reserved,reserved,HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_clk_switch_state_clk_switch_state,clk_switch_state,HI_BBP_LTEDRX_CLK_SWITCH_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_CLK_SWITCH_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_clk_switch_state_reserved,reserved,HI_BBP_LTEDRX_CLK_SWITCH_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_CLK_SWITCH_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_awake_type_awake_type,awake_type,HI_BBP_LTEDRX_AWAKE_TYPE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_AWAKE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_awake_type_reserved,reserved,HI_BBP_LTEDRX_AWAKE_TYPE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_AWAKE_TYPE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_drx_sfn_acc_drx_sfn_acc,drx_sfn_acc,HI_BBP_LTEDRX_DRX_SFN_ACC_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_DRX_SFN_ACC_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_slp_time_cur_slp_time_cur,slp_time_cur,HI_BBP_LTEDRX_SLP_TIME_CUR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SLP_TIME_CUR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_slp_time_cur_reserved,reserved,HI_BBP_LTEDRX_SLP_TIME_CUR_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_SLP_TIME_CUR_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_int_state_switch_int_state,switch_int_state,HI_BBP_LTEDRX_ARM_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_int_state_wakeup_int_state,wakeup_int_state,HI_BBP_LTEDRX_ARM_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_int_state_reserved,reserved,HI_BBP_LTEDRX_ARM_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_sys_cur_timer_lte_sys_cur_timer,lte_sys_cur_timer,HI_BBP_LTEDRX_LTE_SYS_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_SYS_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_sys_cur_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_SYS_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_SYS_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_3412_cur_timer_lte_3412_cur_timer,lte_3412_cur_timer,HI_BBP_LTEDRX_LTE_3412_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_3412_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_3412_cur_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_3412_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_3412_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_vplmn_cur_timer_lte_vplmn_cur_timer,lte_vplmn_cur_timer,HI_BBP_LTEDRX_LTE_VPLMN_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_VPLMN_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_vplmn_cur_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_VPLMN_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_VPLMN_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_tsrhc_cur_timer_lte_tsrhc_cur_timer,lte_tsrhc_cur_timer,HI_BBP_LTEDRX_LTE_TSRHC_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_TSRHC_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_tsrhc_cur_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_TSRHC_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_TSRHC_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_ta_cur_timer_lte_ta_cur_timer,lte_ta_cur_timer,HI_BBP_LTEDRX_LTE_TA_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_TA_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_ta_cur_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_TA_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_TA_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_deeps_cur_timer_lte_deeps_cur_timer,lte_deeps_cur_timer,HI_BBP_LTEDRX_LTE_DEEPS_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_DEEPS_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_deeps_cur_timer_reserved,reserved,HI_BBP_LTEDRX_LTE_DEEPS_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_DEEPS_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_ps32k_cur_timer_lte_ps32k_cur_timer,lte_ps32k_cur_timer,HI_BBP_LTEDRX_LTE_PS32K_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_PS32K_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_lte_ps32kbase_cur_timer_lte_ps32kbase_cur_timer,lte_ps32kbase_cur_timer,HI_BBP_LTEDRX_LTE_PS32KBASE_CUR_TIMER_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_LTE_PS32KBASE_CUR_TIMER_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_state_lte_sys_int_state,lte_sys_int_state,HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_state_lte_3412_int_state,lte_3412_int_state,HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_state_lte_vplmn_int_state,lte_vplmn_int_state,HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_state_lte_tsrhc_int_state,lte_tsrhc_int_state,HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_state_lte_ta_int_state,lte_ta_int_state,HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_state_lte_deeps_int_state,lte_deeps_int_state,HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_state_lte_ps32k_int_state,lte_ps32k_int_state,HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_state_lte_ps32kbase_int_state,lte_ps32kbase_int_state,HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_OFFSET)
HI_SET_GET(hi_bbp_ltedrx_arm_timer_int_state_reserved,reserved,HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_T,HI_BBP_LTEDRX_BASE_ADDR, HI_BBP_LTEDRX_ARM_TIMER_INT_STATE_OFFSET)

#endif

#endif // __HI_BBP_LTEDRX_H__

