/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_bbp_stu.h */
/* Version       : 2.0 */
/* Author        : xxx*/
/* Created       : 2013-02-26*/
/* Last Modified : */
/* Description   :  The C union definition file for the module bbp_stu*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Author        : xxx*/
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_BBP_STU_H__
#define __HI_BBP_STU_H__

/*
 * Project: hi
 * Module : bbp_stu
 */

#ifndef HI_SET_GET
#define HI_SET_GET(a0,a1,a2,a3,a4)
#endif

/********************************************************************************/
/*    bbp_stu 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_BBP_STU_CP_LENGTH_OFFSET                       (0x0) /* CP长度指示寄存器 */
#define    HI_BBP_STU_TIM_SWITCH_OFFSET                      (0x4) /* 系统定时切换指示寄存器 */
#define    HI_BBP_STU_TIM_TRACK_OFFSET                       (0x8) /* 系统定时跟踪指示寄存器 */
#define    HI_BBP_STU_SYS_TIM_OFT_OFFSET                     (0xC) /* 定时调整寄存器 */
#define    HI_BBP_STU_SFN_INIT_OFFSET                        (0x10) /* 系统初始帧号寄存器 */
#define    HI_BBP_STU_SFN_RPT_OFFSET                         (0x14) /* 当前系统帧号寄存器 */
#define    HI_BBP_STU_SUBFRM_NUM_RPT_OFFSET                  (0x18) /* 当前子帧号寄存器 */
#define    HI_BBP_STU_ABS_TIM_RPT_OFFSET                     (0x1C) /* 当前绝对时标寄存器 */
#define    HI_BBP_STU_BACKUP_TIM_VLD_OFFSET                  (0x20) /* 系统定时备份有效标志寄存器 */
#define    HI_BBP_STU_TIM_BACKUP_OFFSET                      (0x24) /* 系统定时备份启动寄存器 */
#define    HI_BBP_STU_BACKUP_TIM_OFT_OFFSET                  (0x28) /* 系统定时备份定时寄存器 */
#define    HI_BBP_STU_BACKUP_SFN_RPT_OFFSET                  (0x2C) /* 系统定时备份帧号寄存器 */
#define    HI_BBP_STU_BBP_INT_RANGE_OFFSET                   (0x30) /* 子帧中断位置寄存器 */
#define    HI_BBP_STU_SYS_TIM_RPT_OFFSET                     (0x34) /* 当前系统定时寄存器 */
#define    HI_BBP_STU_DSP_SYM_INT_POS_OFFSET                 (0x40) /* DSP符号中断位置寄存器 */
#define    HI_BBP_STU_DEM_SYM_POS_OFFSET                     (0x44) /* 解调启动位置寄存器 */
#define    HI_BBP_STU_ARM_SUBFRAME_INT_POS_OFFSET            (0x50) /* ARM子帧中断位置寄存器 */
#define    HI_BBP_STU_ARM_PUB_INT_POS_OFFSET                 (0x54) /* ARM备用定时中断位置寄存器 */
#define    HI_BBP_STU_BBP_VERSION_OFFSET                     (0x70) /* 版本寄存器 */
#define    HI_BBP_STU_BACKUP2_TIM_VLD_OFFSET                 (0x80) /* 无线中继定时有效标志寄存器 */
#define    HI_BBP_STU_TIM_BACKUP2_OFFSET                     (0x84) /* 无线中继定时启动标志寄存器 */
#define    HI_BBP_STU_BACKUP2_TIM_OFT_OFFSET                 (0x88) /* 无线中继定时寄存器 */
#define    HI_BBP_STU_BACKUP2_SFN_RPT_OFFSET                 (0x8C) /* 无线中继定时系统帧号寄存器 */
#define    HI_BBP_STU_AFN_RPT_OFFSET                         (0x90) /* 绝对帧号寄存器 */
#define    HI_BBP_STU_TIM_LOCK_OFFSET                        (0x94) /* 软件锁定指示寄存器 */
#define    HI_BBP_STU_ABS_INIT_OFFSET                        (0x98) /* 测试寄存器 */
#define    HI_BBP_STU_ABS_TIM_ADJ_EN_OFFSET                  (0xA0) /* 绝对时标调整指示寄存器 */
#define    HI_BBP_STU_ABS_TIM_ADJ_OFT_OFFSET                 (0xA4) /* 绝对时标调整寄存器 */
#define    HI_BBP_STU_WIRELESS_RELAY_CFG_OFFSET              (0xA8) /* 无线中继信号控制寄存器 */
#define    HI_BBP_STU_HALF_FRAME_CFG_OFFSET                  (0xAC) /* 寄存器 */
#define    HI_BBP_STU_DEM_TIMER_FROM_FPU_OFFSET              (0xB0) /* 解调定时来源选择寄存器 */
#define    HI_BBP_STU_COEX_LINE_OUT_CTRL_OFFSET              (0xC0) /* 寄存器 */
#define    HI_BBP_STU_COEX_LINE_IN_CTRL_OFFSET               (0xC4) /* WLAN输入控制寄存器 */
#define    HI_BBP_STU_COEX_TIMER_AHEAD_RX_OFFSET             (0xC8) /* 寄存器 */
#define    HI_BBP_STU_COEX_TIMER_DELAY_RX_OFFSET             (0xCC) /* 寄存器 */
#define    HI_BBP_STU_COEX_FRAME_SYNC_PULSEWIDTH_OFFSET      (0xD0) /* 帧头脉冲宽度寄存器 */
#define    HI_BBP_STU_COEX_LTE_RX_TX_ACTIVE_CMD_OFFSET       (0xD4) /* RX和TX信号高低指示寄存器 */
#define    HI_BBP_STU_COEX_WLAN_BT_LD0_EN_CMD_OFFSET         (0xD8) /* 寄存器 */
#define    HI_BBP_STU_COEX_LINE_IN_STATE_OFFSET              (0xDC) /* 寄存器 */
#define    HI_BBP_STU_COEX_FRAME_SYNC_OFT_OFFSET             (0xE0) /* 帧头输出位置寄存器 */
#define    HI_BBP_STU_COEX_OUTPUT_IND_OFFSET                 (0xE4) /* 寄存器 */
#define    HI_BBP_STU_COEX_TIMER_AHEAD_TX_OFFSET             (0xE8) /* 寄存器 */
#define    HI_BBP_STU_COEX_TIMER_DELAY_OFFSET                (0xEC) /* 寄存器 */


#ifndef __ASSEMBLY__

/********************************************************************************/
/*    bbp_stu 寄存器定义（项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    cp_length                  : 1; /* [0..0] CP长度指示：1'b0，短CP；1'b1，长CP配置时刻：任意时刻生效方式：子帧头生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_CP_LENGTH_T;    /* CP长度指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    tim_switch                 : 1; /* [0..0] 系统定时切换有效标志：1'b0，无效；1'b1，有效配置时刻：任意时刻生效方式：立即生效，逻辑须切到新小区最近的子帧头 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_TIM_SWITCH_T;    /* 系统定时切换指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    tim_track                  : 1; /* [0..0] 系统定时跟踪有效标志：1'0，无效；1'b1，有效配置时刻：任意时刻生效方式：立即生效，逻辑须切到新的子帧头 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_TIM_TRACK_T;    /* 系统定时跟踪指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    sys_cyc_oft                : 3; /* [2..0] 待切的系统和解调Ts内定时，相对绝对时标的Ts内偏移，1/4Ts精度配置时刻：任意时刻生效方式：定时切换有效时生效 */
        unsigned int    reserved_2                 : 1; /* [3..3] 保留 */
        unsigned int    sys_ts_oft                 : 15; /* [18..4] 待切的系统和解调帧定时，相对绝对时标的子帧内偏移，Ts精度，0～30719 */
        unsigned int    reserved_1                 : 1; /* [19..19] 保留 */
        unsigned int    sys_sub_oft                : 4; /* [23..20] 待切的系统和解调帧定时，相对绝对时标的子帧间偏移，0～9配置时刻：任意时刻生效方式：定时切换有效时生效 */
        unsigned int    reserved_0                 : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_SYS_TIM_OFT_T;    /* 定时调整寄存器 */

typedef union
{
    struct
    {
        unsigned int    sfn_init                   : 10; /* [9..0] 软件置入的SFN初值配置时刻：任意时刻生效方式：立即生效 */
        unsigned int    reserved_1                 : 6; /* [15..10] 保留 */
        unsigned int    sfn_init_en                : 1; /* [16..16] SFN初值有效指示，1'b1有效。逻辑自清配置时刻：任意时刻生效方式：立即生效 */
        unsigned int    reserved_0                 : 15; /* [31..17] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_SFN_INIT_T;    /* 系统初始帧号寄存器 */

typedef union
{
    struct
    {
        unsigned int    sfn_rpt                    : 10; /* [9..0] 当前帧的SFN值，0～1023上报时刻：系统帧头跳变时刻 */
        unsigned int    reserved                   : 22; /* [31..10] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_SFN_RPT_T;    /* 当前系统帧号寄存器 */

typedef union
{
    struct
    {
        unsigned int    subfrm_num_rpt             : 4; /* [3..0] 当前子帧号，0～9上报时刻：系统子帧头跳变时刻 */
        unsigned int    reserved                   : 28; /* [31..4] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_SUBFRM_NUM_RPT_T;    /* 当前子帧号寄存器 */

typedef union
{
    struct
    {
        unsigned int    abs_cyc_rpt                : 3; /* [2..0] 绝对时标当前计数值的Ts内偏移，1/4Ts精度上报时刻：任意时刻 */
        unsigned int    reserved_2                 : 1; /* [3..3] 保留 */
        unsigned int    abs_ts_rpt                 : 15; /* [18..4] 绝对时标当前计数值的子帧内偏移，Ts精度，0～30719 */
        unsigned int    reserved_1                 : 1; /* [19..19] 保留 */
        unsigned int    abs_sub_rpt                : 4; /* [23..20] 绝对时标当前计数值的子帧间偏移，0～9 */
        unsigned int    reserved_0                 : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_ABS_TIM_RPT_T;    /* 当前绝对时标寄存器 */

typedef union
{
    struct
    {
        unsigned int    backup_tim_vld             : 1; /* [0..0] 系统定时备份有效标志，1'b1有效配置时刻：任意时刻生效方式：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_BACKUP_TIM_VLD_T;    /* 系统定时备份有效标志寄存器 */

typedef union
{
    struct
    {
        unsigned int    tim_backup                 : 1; /* [0..0] 系统定时备份启动标志，1'b1有效。逻辑自清配置时刻：任意时刻生效方式：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_TIM_BACKUP_T;    /* 系统定时备份启动寄存器 */

typedef union
{
    struct
    {
        unsigned int    backup_cyc_oft             : 3; /* [2..0] 备份的系统定时，相对绝对时标的Ts内偏移，1/4Ts精度配置时刻：任意时刻生效方式：立即生效 */
        unsigned int    reserved_2                 : 1; /* [3..3] 保留 */
        unsigned int    backup_ts_oft              : 15; /* [18..4] 备份的系统定时，相对绝对时标的子帧内偏移，Ts精度，0～30719 */
        unsigned int    reserved_1                 : 1; /* [19..19] 保留 */
        unsigned int    backup_sub_oft             : 4; /* [23..20] 备份的系统定时，相对绝对时标的子帧间偏移，0～9配置时刻：任意时刻生效方式：定时切换有效时生效 */
        unsigned int    reserved_0                 : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_BACKUP_TIM_OFT_T;    /* 系统定时备份定时寄存器 */

typedef union
{
    struct
    {
        unsigned int    backup_sfn_rpt             : 10; /* [9..0] 系统定时备份的SFN值，0～1023上报时刻：任意时刻 */
        unsigned int    reserved                   : 22; /* [31..10] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_BACKUP_SFN_RPT_T;    /* 系统定时备份帧号寄存器 */

typedef union
{
    struct
    {
        unsigned int    bbp_int_range              : 8; /* [7..0] 定时中断位置的范围，不包括系统子帧头倒数BBP_INT_RANGE个Ts范围内，32个Ts为单位，缺省值320个Ts；需要固定为0x0A，如调整要与芯片人员确认合法值配置时刻：任意时刻生效方式：立即生效 */
        unsigned int    reserved                   : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_BBP_INT_RANGE_T;    /* 子帧中断位置寄存器 */

typedef union
{
    struct
    {
        unsigned int    sys_cyc_rpt                : 3; /* [2..0] 系统定时当前计数值的Ts内偏移，1/4Ts精度上报时刻：任意时刻 */
        unsigned int    reserved_2                 : 1; /* [3..3] 保留 */
        unsigned int    sys_ts_rpt                 : 15; /* [18..4] 系统定时当前计数值的子帧内偏移，Ts精度，0～30719 */
        unsigned int    reserved_1                 : 1; /* [19..19] 保留 */
        unsigned int    sys_sub_rpt                : 4; /* [23..20] 系统定时当前计数值的子帧间偏移，0～9 */
        unsigned int    reserved_0                 : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_SYS_TIM_RPT_T;    /* 当前系统定时寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_symint_ts_pos          : 12; /* [11..0] 符号中断位置，表示延迟系统定时下行OFDM符号起始时刻的时间，单位为Ts取值范围：16～1615配置时刻：任意时刻生效方式：系统子帧头生效 */
        unsigned int    reserved                   : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_DSP_SYM_INT_POS_T;    /* DSP符号中断位置寄存器 */

typedef union
{
    struct
    {
        unsigned int    dem_sym_pos                : 12; /* [11..0] 解调启动位置，表示延迟系统定时下行OFDM符号起始时刻的时间，单位为Ts取值范围：1096～1615配置时刻：任意时刻生效方式：立即生效 */
        unsigned int    reserved                   : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_DEM_SYM_POS_T;    /* 解调启动位置寄存器 */

typedef union
{
    struct
    {
        unsigned int    arm_subf_int_pos           : 15; /* [14..0] ARM子帧中断的位置，相对下行系统子帧头位置的延时，单位为Ts，取值范围： 0～30143配置时刻：任意时刻生效方式：系统子帧头生效 */
        unsigned int    reserved                   : 17; /* [31..15] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_ARM_SUBFRAME_INT_POS_T;    /* ARM子帧中断位置寄存器 */

typedef union
{
    struct
    {
        unsigned int    arm_pubint_pos             : 15; /* [14..0] ARM备用定时中断的位置，相对下行系统子帧头位置的延时，单位为Ts，取值范围：0～30143配置时刻：任意时刻生效方式：系统子帧头生效 */
        unsigned int    reserved                   : 17; /* [31..15] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_ARM_PUB_INT_POS_T;    /* ARM备用定时中断位置寄存器 */

typedef union
{
    struct
    {
        unsigned int    bbp_version                : 32; /* [31..0] BBP版本寄存器 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_BBP_VERSION_T;    /* 版本寄存器 */

typedef union
{
    struct
    {
        unsigned int    backup2_tim_vld            : 1; /* [0..0] 无线中继定时有效标志，1：有效配置时刻：任意时刻生效方式：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_BACKUP2_TIM_VLD_T;    /* 无线中继定时有效标志寄存器 */

typedef union
{
    struct
    {
        unsigned int    tim_backup2                : 1; /* [0..0] 无线中继定时启动标志，1：有效配置时刻：任意时刻生效方式：系统定时320Ts时刻生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_TIM_BACKUP2_T;    /* 无线中继定时启动标志寄存器 */

typedef union
{
    struct
    {
        unsigned int    backup2_cyc_oft            : 3; /* [2..0] 无线中继定时相对系统定时的Ts内偏移，1/4Ts精度；无线中继定时相对系统定时的子帧间偏移配置为0，子帧内偏移配置为0时，Ts内偏移配置为0；无线中继定时相对系统定时的子帧间偏移配置为9，子帧内偏移配置为9584~30719时，Ts内偏移配置为0~3配置时刻：任意时刻生效方式：TIM_BACKUP2有效时生效 */
        unsigned int    reserved_2                 : 1; /* [3..3] 保留 */
        unsigned int    backup2_ts_oft             : 15; /* [18..4] 无线中继定时相对系统定时的子帧内偏移，Ts精度；无线中继定时相对系统定时的子帧间偏移配置为0时，子帧内偏移配置为0；无线中继定时相对系统定时的子帧间偏移配置为9时，子帧内偏移配置为9584~30719配置时刻：任意时刻生效方式：TIM_BACKUP2有效时生效 */
        unsigned int    reserved_1                 : 1; /* [19..19] 保留 */
        unsigned int    backup2_sub_oft            : 4; /* [23..20] 无线中继定时相对系统定时的子帧间偏移，配置为0或9配置时刻：任意时刻生效方式：TIM_BACKUP2有效时生效 */
        unsigned int    reserved_0                 : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_BACKUP2_TIM_OFT_T;    /* 无线中继定时寄存器 */

typedef union
{
    struct
    {
        unsigned int    backup2_sfn_rpt            : 10; /* [9..0] 无线中继定时的SFN值，0～1023上报时刻：任意时刻 */
        unsigned int    reserved                   : 22; /* [31..10] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_BACKUP2_SFN_RPT_T;    /* 无线中继定时系统帧号寄存器 */

typedef union
{
    struct
    {
        unsigned int    afn_rpt                    : 32; /* [31..0] 绝对帧号 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_AFN_RPT_T;    /* 绝对帧号寄存器 */

typedef union
{
    struct
    {
        unsigned int    tim_lock                   : 1; /* [0..0] 锁定指示（自清），软件指示锁定时STU将AFN、绝对时标、SFN、系统定时、系统子帧号立即锁住并上报 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_TIM_LOCK_T;    /* 软件锁定指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    abs_init_ts_pos            : 15; /* [14..0] 测试寄存器 */
        unsigned int    abs_init_ts_pos_en         : 1; /* [15..15] 测试寄存器 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_ABS_INIT_T;    /* 测试寄存器 */

typedef union
{
    struct
    {
        unsigned int    abs_tim_adj_en             : 1; /* [0..0] 绝对时标相对量调整生效指示，高有效，逻辑自清零，逻辑收到该指示后将当前绝对时标计数值的Ts域加上abs_delta_ts并模30720得到新的Ts计数，并得到子帧进位，子帧进位与当前绝对时标计数值的子帧域和abs_delta_subf相加，结果模10，得到新的子帧间计数值，Ts内计数值保持不变配置时刻：从模需要恢复绝对时标时生效时刻：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_ABS_TIM_ADJ_EN_T;    /* 绝对时标调整指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    abs_delta_ts               : 15; /* [14..0] 绝对时标相对调整量：子帧内Ts调整量，配置范围：0~30719配置时刻：从模需要恢复绝对时标时生效时刻：立即生效 */
        unsigned int    reserved_1                 : 1; /* [15..15] 保留 */
        unsigned int    abs_delta_subf             : 4; /* [19..16] 绝对时标相对调整量：子帧间调整量，单位为子帧，配置范围：0~9配置时刻：从模需要恢复绝对时标时生效时刻：立即生效 */
        unsigned int    reserved_0                 : 12; /* [31..20] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_ABS_TIM_ADJ_OFT_T;    /* 绝对时标调整寄存器 */

typedef union
{
    struct
    {
        unsigned int    relay_multi_sync_mode      : 1; /* [0..0] 无线中继多用途信号模式，1：每帧输出，0：每SFN mod 4 = 0输出配置时刻：任意时刻生效方式：立即生效 */
        unsigned int    reserved_1                 : 15; /* [15..1] 保留 */
        unsigned int    relay_sync_interval        : 2; /* [17..16] 无线中继信号展宽间隔，00：SFN mod 128 = 0展宽01：SFN mod 64 = 0展宽10：SFN mod 256 = 0展宽11：SFN mod 512 = 0展宽配置时刻：任意时刻生效方式：立即生效 */
        unsigned int    reserved_0                 : 14; /* [31..18] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_WIRELESS_RELAY_CFG_T;    /* 无线中继信号控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    cfg_subfrm_num0            : 4; /* [3..0] 指定子帧号0配置时刻：定时切换前生效方式：立即生效 */
        unsigned int    cfg_subfrm_num1            : 4; /* [7..4] 指定子帧号1配置时刻：定时切换前生效方式：立即生效 */
        unsigned int    reserved_1                 : 7; /* [14..8] 保留 */
        unsigned int    half_frame_mask            : 1; /* [15..15] 定时切换后符号中断从(指定子帧+1)开始生效，1：定时切换后符号中断从(指定子帧+1)开始生效0：定时切换后符号中断从下一子帧开始生效配置时刻：定时切换前生效方式：立即生效 */
        unsigned int    reserved_0                 : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_HALF_FRAME_CFG_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dem_timer_from_fpu         : 1; /* [0..0] 解调定时来源，1：来自解调定时1，0：来自解调定时2；默认配置为1配置时刻：上电初始化生效方式：立即生效 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_DEM_TIMER_FROM_FPU_T;    /* 解调定时来源选择寄存器 */

typedef union
{
    struct
    {
        unsigned int    lte_frame_sync_ctrl        : 2; /* [1..0] LTE_Frame_Sync输出信号控制0：输出强制为0；1：输出强制为1；2：逻辑自动控制；3：Invalid */
        unsigned int    reserved_3                 : 2; /* [3..2] 保留 */
        unsigned int    lte_rx_active_ctrl         : 2; /* [5..4] LTE_RX_Active输出信号控制0：输出强制为0；1：输出强制为1；2：逻辑自动控制；3：Invalid */
        unsigned int    reserved_2                 : 2; /* [7..6] 保留 */
        unsigned int    lte_tx_active_ctrl         : 2; /* [9..8] LTE_TX_Active输出信号控制0：输出强制为0；1：输出强制为1；2：逻辑自动控制；3：Invalid */
        unsigned int    reserved_1                 : 2; /* [11..10] 保留 */
        unsigned int    wlan_bt_ldo_en_ctrl        : 2; /* [13..12] WLAN_BT_LDO_EN输出信号控制0：输出强制为0；1：输出强制为1；2：逻辑自动控制；3：Invalid */
        unsigned int    reserved_0                 : 18; /* [31..14] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_LINE_OUT_CTRL_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    wlan_bt_priority_ctrl      : 2; /* [1..0] WLAN_BT_Priority输入信号是否反相控制，此处处理后再上报并传递给上行和中断模块0：不管实际输入，输出强制为0；1：不管实际输入，输出强制为1；2：透传；3：取反 */
        unsigned int    reserved_1                 : 2; /* [3..2] 保留 */
        unsigned int    wlan_bt_tx_active_ctrl     : 2; /* [5..4] WLAN_BT_TX_ Active输入信号是否反相控制，此处处理后再上报并传递给上行和中断模块0：不管实际输入，输出强制为0；1：不管实际输入，输出强制为1；2：透传；3：取反 */
        unsigned int    reserved_0                 : 26; /* [31..6] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_LINE_IN_CTRL_T;    /* WLAN输入控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    coex_t_ahead_rx            : 16; /* [15..0] LTE_RX_Active信号锁存时刻对应的系统定时位置，单位Ts30719>=coex_t_ahead_rx>(30719-9216)Ts */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_TIMER_AHEAD_RX_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    coex_t_delay_rx            : 16; /* [15..0] LTE_RX_Active信号从高变低时对应的系统定时位置，单位Ts0<=coex_t_delay_rx<30000Ts */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_TIMER_DELAY_RX_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    frame_sync_pulsewidth      : 16; /* [15..0] 帧头脉冲宽度，单位Ts1<=frame_sync_pulsewidth<128Ts */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_FRAME_SYNC_PULSEWIDTH_T;    /* 帧头脉冲宽度寄存器 */

typedef union
{
    struct
    {
        unsigned int    coex_lte_rx_active_cmd     : 1; /* [0..0] LTE_RX_Active信号高低指示,1：高0：低 */
        unsigned int    reserved_1                 : 15; /* [15..1] 保留 */
        unsigned int    coex_lte_tx_active_cmd     : 1; /* [16..16] LTE_TX_Active信号高低指示,1：高0：低 */
        unsigned int    reserved_0                 : 15; /* [31..17] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_LTE_RX_TX_ACTIVE_CMD_T;    /* RX和TX信号高低指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    wlan_bt_ldo_en_cmd         : 1; /* [0..0] WLAN_BT_LDO_EN是否与COEX_LTE_RX_ACTIVE_CMD反相指示：0：反相1：不反相 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_WLAN_BT_LD0_EN_CMD_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    wlan_bt_priority_state     : 1; /* [0..0] wlan_bt_priority状态上报，本处上报已经过COEX_LINE_IN_CTRL寄存器指示的操作，0：低1：高 */
        unsigned int    reserved_1                 : 3; /* [3..1] 保留 */
        unsigned int    wlan_bt_tx_active_state    : 1; /* [4..4] wlan_bt_tx_active状态上报，本处上报已经过COEX_LINE_IN_CTRL寄存器指示的操作，0：低1：高 */
        unsigned int    reserved_0                 : 27; /* [31..5] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_LINE_IN_STATE_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    coex_frame_sync_ts_oft     : 15; /* [14..0] LTE_Frame_Sync输出信号在系统定时中的位置：子帧内Ts位置，配置范围：0~30143配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    reserved_1                 : 1; /* [15..15] 保留 */
        unsigned int    coex_frame_sync_subfrm_oft : 4; /* [19..16] LTE_Frame_Sync输出信号在系统定时中的位置：子帧位置，单位为子帧，配置范围：0~9配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    reserved_0                 : 12; /* [31..20] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_FRAME_SYNC_OFT_T;    /* 帧头输出位置寄存器 */

typedef union
{
    struct
    {
        unsigned int    wlan_bt_rx_active_ind      : 1; /* [0..0] lte_rx_active输出信号有效指示：1：正脉冲有效0：负脉冲有效配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    wlan_bt_lte_frame_sync_ind : 1; /* [1..1] lte_frame_sync输出信号有效指示：1：正脉冲有效0：负脉冲有效配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    reserved_1                 : 14; /* [15..2]  */
        unsigned int    wlan_bt_tx_active_ind      : 1; /* [16..16] lte_tx_active输出信号有效指示：1：正脉冲有效0：负脉冲有效配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    reserved_0                 : 15; /* [31..17] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_OUTPUT_IND_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    coex_t_ahead_tx            : 16; /* [15..0] LTE_TX_Active信号锁存时刻对应的系统定时位置，单位Ts30719>=coex_t_ahead_tx>(30719-9216)Ts */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_TIMER_AHEAD_TX_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    coex_t_delay_tx            : 16; /* [15..0] LTE_TX_Active信号从高变低时对应的系统定时位置，单位Ts0<=coex_t_delay_tx<30000Ts */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_STU_COEX_TIMER_DELAY_T;    /* 寄存器 */


/********************************************************************************/
/*    bbp_stu 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
HI_SET_GET(hi_bbp_stu_cp_length_cp_length,cp_length,HI_BBP_STU_CP_LENGTH_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_CP_LENGTH_OFFSET)
HI_SET_GET(hi_bbp_stu_cp_length_reserved,reserved,HI_BBP_STU_CP_LENGTH_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_CP_LENGTH_OFFSET)
HI_SET_GET(hi_bbp_stu_tim_switch_tim_switch,tim_switch,HI_BBP_STU_TIM_SWITCH_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_TIM_SWITCH_OFFSET)
HI_SET_GET(hi_bbp_stu_tim_switch_reserved,reserved,HI_BBP_STU_TIM_SWITCH_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_TIM_SWITCH_OFFSET)
HI_SET_GET(hi_bbp_stu_tim_track_tim_track,tim_track,HI_BBP_STU_TIM_TRACK_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_TIM_TRACK_OFFSET)
HI_SET_GET(hi_bbp_stu_tim_track_reserved,reserved,HI_BBP_STU_TIM_TRACK_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_TIM_TRACK_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_oft_sys_cyc_oft,sys_cyc_oft,HI_BBP_STU_SYS_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_oft_reserved_2,reserved_2,HI_BBP_STU_SYS_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_oft_sys_ts_oft,sys_ts_oft,HI_BBP_STU_SYS_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_oft_reserved_1,reserved_1,HI_BBP_STU_SYS_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_oft_sys_sub_oft,sys_sub_oft,HI_BBP_STU_SYS_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_oft_reserved_0,reserved_0,HI_BBP_STU_SYS_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_sfn_init_sfn_init,sfn_init,HI_BBP_STU_SFN_INIT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SFN_INIT_OFFSET)
HI_SET_GET(hi_bbp_stu_sfn_init_reserved_1,reserved_1,HI_BBP_STU_SFN_INIT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SFN_INIT_OFFSET)
HI_SET_GET(hi_bbp_stu_sfn_init_sfn_init_en,sfn_init_en,HI_BBP_STU_SFN_INIT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SFN_INIT_OFFSET)
HI_SET_GET(hi_bbp_stu_sfn_init_reserved_0,reserved_0,HI_BBP_STU_SFN_INIT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SFN_INIT_OFFSET)
HI_SET_GET(hi_bbp_stu_sfn_rpt_sfn_rpt,sfn_rpt,HI_BBP_STU_SFN_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SFN_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_sfn_rpt_reserved,reserved,HI_BBP_STU_SFN_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SFN_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_subfrm_num_rpt_subfrm_num_rpt,subfrm_num_rpt,HI_BBP_STU_SUBFRM_NUM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SUBFRM_NUM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_subfrm_num_rpt_reserved,reserved,HI_BBP_STU_SUBFRM_NUM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SUBFRM_NUM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_rpt_abs_cyc_rpt,abs_cyc_rpt,HI_BBP_STU_ABS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_rpt_reserved_2,reserved_2,HI_BBP_STU_ABS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_rpt_abs_ts_rpt,abs_ts_rpt,HI_BBP_STU_ABS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_rpt_reserved_1,reserved_1,HI_BBP_STU_ABS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_rpt_abs_sub_rpt,abs_sub_rpt,HI_BBP_STU_ABS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_rpt_reserved_0,reserved_0,HI_BBP_STU_ABS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup_tim_vld_backup_tim_vld,backup_tim_vld,HI_BBP_STU_BACKUP_TIM_VLD_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP_TIM_VLD_OFFSET)
HI_SET_GET(hi_bbp_stu_backup_tim_vld_reserved,reserved,HI_BBP_STU_BACKUP_TIM_VLD_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP_TIM_VLD_OFFSET)
HI_SET_GET(hi_bbp_stu_tim_backup_tim_backup,tim_backup,HI_BBP_STU_TIM_BACKUP_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_TIM_BACKUP_OFFSET)
HI_SET_GET(hi_bbp_stu_tim_backup_reserved,reserved,HI_BBP_STU_TIM_BACKUP_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_TIM_BACKUP_OFFSET)
HI_SET_GET(hi_bbp_stu_backup_tim_oft_backup_cyc_oft,backup_cyc_oft,HI_BBP_STU_BACKUP_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup_tim_oft_reserved_2,reserved_2,HI_BBP_STU_BACKUP_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup_tim_oft_backup_ts_oft,backup_ts_oft,HI_BBP_STU_BACKUP_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup_tim_oft_reserved_1,reserved_1,HI_BBP_STU_BACKUP_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup_tim_oft_backup_sub_oft,backup_sub_oft,HI_BBP_STU_BACKUP_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup_tim_oft_reserved_0,reserved_0,HI_BBP_STU_BACKUP_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup_sfn_rpt_backup_sfn_rpt,backup_sfn_rpt,HI_BBP_STU_BACKUP_SFN_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP_SFN_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup_sfn_rpt_reserved,reserved,HI_BBP_STU_BACKUP_SFN_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP_SFN_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_bbp_int_range_bbp_int_range,bbp_int_range,HI_BBP_STU_BBP_INT_RANGE_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BBP_INT_RANGE_OFFSET)
HI_SET_GET(hi_bbp_stu_bbp_int_range_reserved,reserved,HI_BBP_STU_BBP_INT_RANGE_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BBP_INT_RANGE_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_rpt_sys_cyc_rpt,sys_cyc_rpt,HI_BBP_STU_SYS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_rpt_reserved_2,reserved_2,HI_BBP_STU_SYS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_rpt_sys_ts_rpt,sys_ts_rpt,HI_BBP_STU_SYS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_rpt_reserved_1,reserved_1,HI_BBP_STU_SYS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_rpt_sys_sub_rpt,sys_sub_rpt,HI_BBP_STU_SYS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_sys_tim_rpt_reserved_0,reserved_0,HI_BBP_STU_SYS_TIM_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_SYS_TIM_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_dsp_sym_int_pos_dsp_symint_ts_pos,dsp_symint_ts_pos,HI_BBP_STU_DSP_SYM_INT_POS_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_DSP_SYM_INT_POS_OFFSET)
HI_SET_GET(hi_bbp_stu_dsp_sym_int_pos_reserved,reserved,HI_BBP_STU_DSP_SYM_INT_POS_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_DSP_SYM_INT_POS_OFFSET)
HI_SET_GET(hi_bbp_stu_dem_sym_pos_dem_sym_pos,dem_sym_pos,HI_BBP_STU_DEM_SYM_POS_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_DEM_SYM_POS_OFFSET)
HI_SET_GET(hi_bbp_stu_dem_sym_pos_reserved,reserved,HI_BBP_STU_DEM_SYM_POS_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_DEM_SYM_POS_OFFSET)
HI_SET_GET(hi_bbp_stu_arm_subframe_int_pos_arm_subf_int_pos,arm_subf_int_pos,HI_BBP_STU_ARM_SUBFRAME_INT_POS_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ARM_SUBFRAME_INT_POS_OFFSET)
HI_SET_GET(hi_bbp_stu_arm_subframe_int_pos_reserved,reserved,HI_BBP_STU_ARM_SUBFRAME_INT_POS_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ARM_SUBFRAME_INT_POS_OFFSET)
HI_SET_GET(hi_bbp_stu_arm_pub_int_pos_arm_pubint_pos,arm_pubint_pos,HI_BBP_STU_ARM_PUB_INT_POS_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ARM_PUB_INT_POS_OFFSET)
HI_SET_GET(hi_bbp_stu_arm_pub_int_pos_reserved,reserved,HI_BBP_STU_ARM_PUB_INT_POS_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ARM_PUB_INT_POS_OFFSET)
HI_SET_GET(hi_bbp_stu_bbp_version_bbp_version,bbp_version,HI_BBP_STU_BBP_VERSION_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BBP_VERSION_OFFSET)
HI_SET_GET(hi_bbp_stu_backup2_tim_vld_backup2_tim_vld,backup2_tim_vld,HI_BBP_STU_BACKUP2_TIM_VLD_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP2_TIM_VLD_OFFSET)
HI_SET_GET(hi_bbp_stu_backup2_tim_vld_reserved,reserved,HI_BBP_STU_BACKUP2_TIM_VLD_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP2_TIM_VLD_OFFSET)
HI_SET_GET(hi_bbp_stu_tim_backup2_tim_backup2,tim_backup2,HI_BBP_STU_TIM_BACKUP2_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_TIM_BACKUP2_OFFSET)
HI_SET_GET(hi_bbp_stu_tim_backup2_reserved,reserved,HI_BBP_STU_TIM_BACKUP2_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_TIM_BACKUP2_OFFSET)
HI_SET_GET(hi_bbp_stu_backup2_tim_oft_backup2_cyc_oft,backup2_cyc_oft,HI_BBP_STU_BACKUP2_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP2_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup2_tim_oft_reserved_2,reserved_2,HI_BBP_STU_BACKUP2_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP2_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup2_tim_oft_backup2_ts_oft,backup2_ts_oft,HI_BBP_STU_BACKUP2_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP2_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup2_tim_oft_reserved_1,reserved_1,HI_BBP_STU_BACKUP2_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP2_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup2_tim_oft_backup2_sub_oft,backup2_sub_oft,HI_BBP_STU_BACKUP2_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP2_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup2_tim_oft_reserved_0,reserved_0,HI_BBP_STU_BACKUP2_TIM_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP2_TIM_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup2_sfn_rpt_backup2_sfn_rpt,backup2_sfn_rpt,HI_BBP_STU_BACKUP2_SFN_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP2_SFN_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_backup2_sfn_rpt_reserved,reserved,HI_BBP_STU_BACKUP2_SFN_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_BACKUP2_SFN_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_afn_rpt_afn_rpt,afn_rpt,HI_BBP_STU_AFN_RPT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_AFN_RPT_OFFSET)
HI_SET_GET(hi_bbp_stu_tim_lock_tim_lock,tim_lock,HI_BBP_STU_TIM_LOCK_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_TIM_LOCK_OFFSET)
HI_SET_GET(hi_bbp_stu_tim_lock_reserved,reserved,HI_BBP_STU_TIM_LOCK_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_TIM_LOCK_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_init_abs_init_ts_pos,abs_init_ts_pos,HI_BBP_STU_ABS_INIT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_INIT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_init_abs_init_ts_pos_en,abs_init_ts_pos_en,HI_BBP_STU_ABS_INIT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_INIT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_init_reserved,reserved,HI_BBP_STU_ABS_INIT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_INIT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_adj_en_abs_tim_adj_en,abs_tim_adj_en,HI_BBP_STU_ABS_TIM_ADJ_EN_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_ADJ_EN_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_adj_en_reserved,reserved,HI_BBP_STU_ABS_TIM_ADJ_EN_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_ADJ_EN_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_adj_oft_abs_delta_ts,abs_delta_ts,HI_BBP_STU_ABS_TIM_ADJ_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_ADJ_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_adj_oft_reserved_1,reserved_1,HI_BBP_STU_ABS_TIM_ADJ_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_ADJ_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_adj_oft_abs_delta_subf,abs_delta_subf,HI_BBP_STU_ABS_TIM_ADJ_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_ADJ_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_abs_tim_adj_oft_reserved_0,reserved_0,HI_BBP_STU_ABS_TIM_ADJ_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_ABS_TIM_ADJ_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_wireless_relay_cfg_relay_multi_sync_mode,relay_multi_sync_mode,HI_BBP_STU_WIRELESS_RELAY_CFG_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_WIRELESS_RELAY_CFG_OFFSET)
HI_SET_GET(hi_bbp_stu_wireless_relay_cfg_reserved_1,reserved_1,HI_BBP_STU_WIRELESS_RELAY_CFG_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_WIRELESS_RELAY_CFG_OFFSET)
HI_SET_GET(hi_bbp_stu_wireless_relay_cfg_relay_sync_interval,relay_sync_interval,HI_BBP_STU_WIRELESS_RELAY_CFG_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_WIRELESS_RELAY_CFG_OFFSET)
HI_SET_GET(hi_bbp_stu_wireless_relay_cfg_reserved_0,reserved_0,HI_BBP_STU_WIRELESS_RELAY_CFG_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_WIRELESS_RELAY_CFG_OFFSET)
HI_SET_GET(hi_bbp_stu_half_frame_cfg_cfg_subfrm_num0,cfg_subfrm_num0,HI_BBP_STU_HALF_FRAME_CFG_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_HALF_FRAME_CFG_OFFSET)
HI_SET_GET(hi_bbp_stu_half_frame_cfg_cfg_subfrm_num1,cfg_subfrm_num1,HI_BBP_STU_HALF_FRAME_CFG_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_HALF_FRAME_CFG_OFFSET)
HI_SET_GET(hi_bbp_stu_half_frame_cfg_reserved_1,reserved_1,HI_BBP_STU_HALF_FRAME_CFG_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_HALF_FRAME_CFG_OFFSET)
HI_SET_GET(hi_bbp_stu_half_frame_cfg_half_frame_mask,half_frame_mask,HI_BBP_STU_HALF_FRAME_CFG_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_HALF_FRAME_CFG_OFFSET)
HI_SET_GET(hi_bbp_stu_half_frame_cfg_reserved_0,reserved_0,HI_BBP_STU_HALF_FRAME_CFG_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_HALF_FRAME_CFG_OFFSET)
HI_SET_GET(hi_bbp_stu_dem_timer_from_fpu_dem_timer_from_fpu,dem_timer_from_fpu,HI_BBP_STU_DEM_TIMER_FROM_FPU_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_DEM_TIMER_FROM_FPU_OFFSET)
HI_SET_GET(hi_bbp_stu_dem_timer_from_fpu_reserved,reserved,HI_BBP_STU_DEM_TIMER_FROM_FPU_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_DEM_TIMER_FROM_FPU_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_out_ctrl_lte_frame_sync_ctrl,lte_frame_sync_ctrl,HI_BBP_STU_COEX_LINE_OUT_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_OUT_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_out_ctrl_reserved_3,reserved_3,HI_BBP_STU_COEX_LINE_OUT_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_OUT_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_out_ctrl_lte_rx_active_ctrl,lte_rx_active_ctrl,HI_BBP_STU_COEX_LINE_OUT_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_OUT_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_out_ctrl_reserved_2,reserved_2,HI_BBP_STU_COEX_LINE_OUT_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_OUT_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_out_ctrl_lte_tx_active_ctrl,lte_tx_active_ctrl,HI_BBP_STU_COEX_LINE_OUT_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_OUT_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_out_ctrl_reserved_1,reserved_1,HI_BBP_STU_COEX_LINE_OUT_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_OUT_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_out_ctrl_wlan_bt_ldo_en_ctrl,wlan_bt_ldo_en_ctrl,HI_BBP_STU_COEX_LINE_OUT_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_OUT_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_out_ctrl_reserved_0,reserved_0,HI_BBP_STU_COEX_LINE_OUT_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_OUT_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_in_ctrl_wlan_bt_priority_ctrl,wlan_bt_priority_ctrl,HI_BBP_STU_COEX_LINE_IN_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_IN_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_in_ctrl_reserved_1,reserved_1,HI_BBP_STU_COEX_LINE_IN_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_IN_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_in_ctrl_wlan_bt_tx_active_ctrl,wlan_bt_tx_active_ctrl,HI_BBP_STU_COEX_LINE_IN_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_IN_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_in_ctrl_reserved_0,reserved_0,HI_BBP_STU_COEX_LINE_IN_CTRL_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_IN_CTRL_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_timer_ahead_rx_coex_t_ahead_rx,coex_t_ahead_rx,HI_BBP_STU_COEX_TIMER_AHEAD_RX_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_TIMER_AHEAD_RX_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_timer_ahead_rx_reserved,reserved,HI_BBP_STU_COEX_TIMER_AHEAD_RX_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_TIMER_AHEAD_RX_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_timer_delay_rx_coex_t_delay_rx,coex_t_delay_rx,HI_BBP_STU_COEX_TIMER_DELAY_RX_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_TIMER_DELAY_RX_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_timer_delay_rx_reserved,reserved,HI_BBP_STU_COEX_TIMER_DELAY_RX_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_TIMER_DELAY_RX_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_frame_sync_pulsewidth_frame_sync_pulsewidth,frame_sync_pulsewidth,HI_BBP_STU_COEX_FRAME_SYNC_PULSEWIDTH_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_FRAME_SYNC_PULSEWIDTH_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_frame_sync_pulsewidth_reserved,reserved,HI_BBP_STU_COEX_FRAME_SYNC_PULSEWIDTH_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_FRAME_SYNC_PULSEWIDTH_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_lte_rx_tx_active_cmd_coex_lte_rx_active_cmd,coex_lte_rx_active_cmd,HI_BBP_STU_COEX_LTE_RX_TX_ACTIVE_CMD_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LTE_RX_TX_ACTIVE_CMD_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_lte_rx_tx_active_cmd_reserved_1,reserved_1,HI_BBP_STU_COEX_LTE_RX_TX_ACTIVE_CMD_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LTE_RX_TX_ACTIVE_CMD_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_lte_rx_tx_active_cmd_coex_lte_tx_active_cmd,coex_lte_tx_active_cmd,HI_BBP_STU_COEX_LTE_RX_TX_ACTIVE_CMD_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LTE_RX_TX_ACTIVE_CMD_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_lte_rx_tx_active_cmd_reserved_0,reserved_0,HI_BBP_STU_COEX_LTE_RX_TX_ACTIVE_CMD_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LTE_RX_TX_ACTIVE_CMD_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_wlan_bt_ld0_en_cmd_wlan_bt_ldo_en_cmd,wlan_bt_ldo_en_cmd,HI_BBP_STU_COEX_WLAN_BT_LD0_EN_CMD_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_WLAN_BT_LD0_EN_CMD_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_wlan_bt_ld0_en_cmd_reserved,reserved,HI_BBP_STU_COEX_WLAN_BT_LD0_EN_CMD_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_WLAN_BT_LD0_EN_CMD_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_in_state_wlan_bt_priority_state,wlan_bt_priority_state,HI_BBP_STU_COEX_LINE_IN_STATE_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_IN_STATE_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_in_state_reserved_1,reserved_1,HI_BBP_STU_COEX_LINE_IN_STATE_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_IN_STATE_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_in_state_wlan_bt_tx_active_state,wlan_bt_tx_active_state,HI_BBP_STU_COEX_LINE_IN_STATE_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_IN_STATE_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_line_in_state_reserved_0,reserved_0,HI_BBP_STU_COEX_LINE_IN_STATE_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_LINE_IN_STATE_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_frame_sync_oft_coex_frame_sync_ts_oft,coex_frame_sync_ts_oft,HI_BBP_STU_COEX_FRAME_SYNC_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_FRAME_SYNC_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_frame_sync_oft_reserved_1,reserved_1,HI_BBP_STU_COEX_FRAME_SYNC_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_FRAME_SYNC_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_frame_sync_oft_coex_frame_sync_subfrm_oft,coex_frame_sync_subfrm_oft,HI_BBP_STU_COEX_FRAME_SYNC_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_FRAME_SYNC_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_frame_sync_oft_reserved_0,reserved_0,HI_BBP_STU_COEX_FRAME_SYNC_OFT_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_FRAME_SYNC_OFT_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_output_ind_wlan_bt_rx_active_ind,wlan_bt_rx_active_ind,HI_BBP_STU_COEX_OUTPUT_IND_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_OUTPUT_IND_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_output_ind_wlan_bt_lte_frame_sync_ind,wlan_bt_lte_frame_sync_ind,HI_BBP_STU_COEX_OUTPUT_IND_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_OUTPUT_IND_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_output_ind_reserved_1,reserved_1,HI_BBP_STU_COEX_OUTPUT_IND_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_OUTPUT_IND_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_output_ind_wlan_bt_tx_active_ind,wlan_bt_tx_active_ind,HI_BBP_STU_COEX_OUTPUT_IND_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_OUTPUT_IND_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_output_ind_reserved_0,reserved_0,HI_BBP_STU_COEX_OUTPUT_IND_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_OUTPUT_IND_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_timer_ahead_tx_coex_t_ahead_tx,coex_t_ahead_tx,HI_BBP_STU_COEX_TIMER_AHEAD_TX_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_TIMER_AHEAD_TX_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_timer_ahead_tx_reserved,reserved,HI_BBP_STU_COEX_TIMER_AHEAD_TX_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_TIMER_AHEAD_TX_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_timer_delay_coex_t_delay_tx,coex_t_delay_tx,HI_BBP_STU_COEX_TIMER_DELAY_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_TIMER_DELAY_OFFSET)
HI_SET_GET(hi_bbp_stu_coex_timer_delay_reserved,reserved,HI_BBP_STU_COEX_TIMER_DELAY_T,HI_BBP_STU_BASE_ADDR, HI_BBP_STU_COEX_TIMER_DELAY_OFFSET)

#endif

#endif // __HI_BBP_STU_H__

