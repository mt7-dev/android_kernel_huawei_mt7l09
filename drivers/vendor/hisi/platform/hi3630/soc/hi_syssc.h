/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_syssc.h */
/* Version       : 2.0 */
/* Author        : xujingcui*/
/* Created       : 2013-09-25*/
/* Last Modified : */
/* Description   :  The C union definition file for the module syssc*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Author        : xujingcui*/
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_SYSSC_H__
#define __HI_SYSSC_H__

/*
 * Project: hi
 * Module : syssc
 */

#ifndef HI_SET_GET
#define HI_SET_GET(a0,a1,a2,a3,a4)
#endif

/********************************************************************************/
/*    syssc 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_CRG_CLKEN3_OFFSET                              (0x18) /* 时钟使能3 */
#define    HI_CRG_CLKDIS3_OFFSET                             (0x1C) /* 时钟关闭3 */
#define    HI_CRG_CLKSTAT3_OFFSET                            (0x20) /* 时钟状态3 */
#define    HI_CRG_CLKEN4_OFFSET                              (0x24) /* 时钟使能4 */
#define    HI_CRG_CLKDIS4_OFFSET                             (0x28) /* 时钟关闭4 */
#define    HI_CRG_CLKSTAT4_OFFSET                            (0x2C) /* 时钟控制状态4 */
#define    HI_CRG_CLKEN5_OFFSET                              (0x30) /* 时钟使能5（BBP） */
#define    HI_CRG_CLKDIS5_OFFSET                             (0x34) /* 时钟关闭控制寄存器0(BBP) */
#define    HI_CRG_CLKSTAT5_OFFSET                            (0x38) /* 时钟状态控制寄存器0(BBP) */
#define    HI_CRG_SRSTEN1_OFFSET                             (0x60) /* 软复位请求使能1 */
#define    HI_CRG_SRSTDIS1_OFFSET                            (0x64) /* 软复位请求关闭1 */
#define    HI_CRG_SRSTSTAT1_OFFSET                           (0x68) /* 软复位请求状态1 */
#define    HI_CRG_SRSTEN3_OFFSET                             (0x78) /* 软复位请求使能3（ABB+BBP） */
#define    HI_CRG_SRSTDIS3_OFFSET                            (0x7C) /* 软复位请求关闭3（ABB+BBP） */
#define    HI_CRG_SRSTSTAT3_OFFSET                           (0x80) /* 软复位请求状态3（ABB+BBP） */
#define    HI_CRG_CLKDIV2_OFFSET                             (0x104) /* 时钟分频控制2(总线分频） */
#define    HI_CRG_CLKDIV5_OFFSET                             (0x110) /* 时钟分频控制5(BBEDSP/CIPHER时钟分频) */
#define    HI_CRG_CLK_SEL2_OFFSET                            (0x144) /* 时钟源选择控制2(TIMER) */
#define    HI_CRG_CLK_SEL3_OFFSET                            (0x148) /* 时钟源选择控制3（BBP） */
#define    HI_CRG_A9PLL_CFG0_OFFSET                          (0x200) /* A9PLL配置寄存器0 */
#define    HI_CRG_A9PLL_CFG1_OFFSET                          (0x204) /* A9PLL配置寄存器1 */
#define    HI_CRG_A9PLL_CFG2_OFFSET                          (0x208) /* A9PLL配置寄存器2 */
#define    HI_CRG_DSPPLL_CFG0_OFFSET                         (0x20C) /* BBEPLL配置寄存器0 */
#define    HI_CRG_DSPPLL_CFG1_OFFSET                         (0x210) /* BBEPLL配置寄存器1 */
#define    HI_CRG_DSPPLL_CFG2_OFFSET                         (0x214) /* BBEPLL配置寄存器2 */
#define    HI_CRG_BBPPLL_CFG0_OFFSET                         (0x224) /* BBPPLL配置寄存器0 */
#define    HI_CRG_BBPPLL_CFG1_OFFSET                         (0x228) /* BBPPLL配置寄存器1 */
#define    HI_CRG_BBPPLL_CFG2_OFFSET                         (0x22C) /* BBPPLL配置寄存器2 */
#define    HI_SC_CTRL0_OFFSET                                (0x400) /* ModemA9启动Remap控制寄存器 */
#define    HI_SC_CTRL2_OFFSET                                (0x408) /* 看门狗配置寄存器。 */
#define    HI_SC_CTRL3_OFFSET                                (0x40C) /* 外设配置寄存器。 */
#define    HI_SC_CTRL4_OFFSET                                (0x410) /* CICOM加解密模式配置寄存器。 */
#define    HI_SC_CTRL5_OFFSET                                (0x414) /* 保留寄存器 */
#define    HI_SC_CTRL6_OFFSET                                (0x418) /* 保留寄存器 */
#define    HI_SC_CTRL9_OFFSET                                (0x424) /* BBE16启动地址配置寄存器 */
#define    HI_SC_CTRL10_OFFSET                               (0x428) /* 保留寄存器 */
#define    HI_SC_CTRL11_OFFSET                               (0x42C) /* Modem A9配置寄存器。 */
#define    HI_SC_CTRL12_OFFSET                               (0x430) /* 保留寄存器 */
#define    HI_SC_CTRL13_OFFSET                               (0x434) /* BBE16配置寄存器 */
#define    HI_SC_CTRL17_OFFSET                               (0x444) /* HPM控制寄存器 */
#define    HI_SC_CTRL20_OFFSET                               (0x450) /* 自动门控时钟控制寄存器0 */
#define    HI_SC_CTRL21_OFFSET                               (0x454) /* 自动门控时钟控制寄存器1 */
#define    HI_SC_CTRL22_OFFSET                               (0x458) /* BBE16 memory 时序配置寄存器 */
#define    HI_SC_CTRL23_OFFSET                               (0x45C) /* SOC memory 时序配置寄存器 */
#define    HI_SC_CTRL24_OFFSET                               (0x460) /* memory 时序配置寄存器2（CCPU+L2)。 */
#define    HI_SC_CTRL25_OFFSET                               (0x464) /* SOC 测试信号选择寄存器(保留） */
#define    HI_SC_CTRL26_OFFSET                               (0x468) /* SOC 测试信号bypass寄存器(保留） */
#define    HI_SC_CTRL28_OFFSET                               (0x470) /* 地址过滤首地址(ACP空间起始地址) */
#define    HI_SC_CTRL29_OFFSET                               (0x474) /* 地址过滤尾地址(ACP空间结束地址) */
#define    HI_SC_CTRL30_OFFSET                               (0x478) /* 保留寄存器 */
#define    HI_SC_CTRL45_OFFSET                               (0x4B4) /* uicc控制寄存器 */
#define    HI_SC_CTRL52_OFFSET                               (0x4D0) /* BBE16不可屏蔽中断寄存器。 */
#define    HI_SC_CTRL55_OFFSET                               (0x4DC) /* memory低功耗配置寄存器0 */
#define    HI_SC_CTRL56_OFFSET                               (0x4E0) /* BBP LBUS控制寄存器。 */
#define    HI_SC_CTRL57_OFFSET                               (0x4E4) /* BBP保留控制寄存器。 */
#define    HI_SC_CTRL68_OFFSET                               (0x510) /* memory低功耗配置寄存器2 */
#define    HI_SC_CTRL69_OFFSET                               (0x514) /* 系统错误地址访问功能寄存器 */
#define    HI_SC_CTRL70_OFFSET                               (0x518) /* 互联优先级配置寄存器0 */
#define    HI_SC_CTRL71_OFFSET                               (0x51C) /* 互联优先级配置寄存器1 */
#define    HI_SC_CTRL72_OFFSET                               (0x520) /* 互联优先级配置寄存器2（DW_axi_mst) */
#define    HI_SC_CTRL73_OFFSET                               (0x524) /* 加速器访问空间控制寄存器0 */
#define    HI_SC_CTRL74_OFFSET                               (0x528) /* 加速器访问空间控制寄存器1 */
#define    HI_SC_CTRL75_OFFSET                               (0x52C) /* 加速器访问空间控制寄存器2 */
#define    HI_SC_CTRL76_OFFSET                               (0x530) /* 加速器访问空间控制寄存器3 */
#define    HI_SC_CTRL77_OFFSET                               (0x534) /* 加速器访问空间控制寄存器4 */
#define    HI_SC_CTRL78_OFFSET                               (0x538) /* 加速器访问空间控制寄存器5 */
#define    HI_SC_CTRL79_OFFSET                               (0x53C) /* 加速器访问空间控制寄存器6 */
#define    HI_SC_CTRL80_OFFSET                               (0x540) /* 加速器访问空间控制寄存器7 */
#define    HI_SC_CTRL103_OFFSET                              (0x59C) /* slave active屏蔽寄存器0 */
#define    HI_SC_CTRL104_OFFSET                              (0x5A0) /* 保留寄存器 */
#define    HI_SC_CTRL105_OFFSET                              (0x5A4) /* slave active屏蔽寄存器2 */
#define    HI_SC_STAT1_OFFSET                                (0x604) /* Modem A9状态寄存器 */
#define    HI_SC_STAT2_OFFSET                                (0x608) /* BBE16 DSP状态寄存器 */
#define    HI_SC_STAT3_OFFSET                                (0x60C) /* HPM状态寄存器 */
#define    HI_SC_STAT5_OFFSET                                (0x614) /* BBE16中断状态寄存器 */
#define    HI_SC_STAT6_OFFSET                                (0x618) /* Modem A9校验状态寄存器 */
#define    HI_SC_STAT9_OFFSET                                (0x624) /* AXI桥锁死状态寄存器0 */
#define    HI_SC_STAT10_OFFSET                               (0x628) /* AXI桥锁死状态寄存器1 */
#define    HI_SC_STAT15_OFFSET                               (0x63C) /* slave active状态寄存器 */
#define    HI_SC_STAT22_OFFSET                               (0x658) /* AXI桥锁死状态寄存器2(BBPHY) */
#define    HI_SC_STAT26_OFFSET                               (0x668) /* master状态寄存器 */
#define    HI_SC_STAT27_OFFSET                               (0x66C) /* slave active状态寄存器0 */
#define    HI_SC_STAT29_OFFSET                               (0x674) /* slave active状态寄存器2 */
#define    HI_SC_STAT32_OFFSET                               (0x680) /* BBP保留状态寄存器。 */
#define    HI_SC_STAT35_OFFSET                               (0x68C) /* 安全Key寄存器0 */
#define    HI_SC_STAT36_OFFSET                               (0x690) /* 安全Key寄存器1 */
#define    HI_SC_STAT37_OFFSET                               (0x694) /* 安全Key寄存器2 */
#define    HI_SC_STAT38_OFFSET                               (0x698) /* 安全Key寄存器3 */
#define    HI_SC_STAT41_OFFSET                               (0x6A4) /* master访问错误状态寄存器0。 */
#define    HI_SC_STAT42_OFFSET                               (0x6A8) /* master访问错误状态寄存器1。 */
#define    HI_SC_STAT43_OFFSET                               (0x6AC) /* ahb外设访问错误地址寄存器。 */
#define    HI_SC_STAT44_OFFSET                               (0x6B0) /* apb外设访问错误地址寄存器。 */
#define    HI_SC_STAT46_OFFSET                               (0x6B8) /* glb桥m1口（dw_axi_bbphy）写访问错误地址寄存器。 */
#define    HI_SC_STAT47_OFFSET                               (0x6BC) /* glb桥m1口（dw_axi_bbphy）读访问错误地址寄存器。 */
#define    HI_SC_STAT48_OFFSET                               (0x6C0) /* glb桥m2口（dw_axi_mst）写访问错误地址寄存器。 */
#define    HI_SC_STAT49_OFFSET                               (0x6C4) /* glb桥m2口（dw_axi_mst）读访问错误地址寄存器。 */
#define    HI_SC_STAT50_OFFSET                               (0x6C8) /* glb桥m3口（app a9 m0口）写访问错误地址寄存器。 */
#define    HI_SC_STAT51_OFFSET                               (0x6CC) /* glb桥m3口（app a9 m0口）读访问错误地址寄存器。 */
#define    HI_SC_STAT52_OFFSET                               (0x6D0) /* glb桥m4口（mdm a9 m0口）写访问错误地址寄存器。 */
#define    HI_SC_STAT53_OFFSET                               (0x6D4) /* glb桥m4口（mdm a9 m0口）读访问错误地址寄存器。 */
#define    HI_SC_STAT54_OFFSET                               (0x6D8) /* glb桥m5口（reserved）写访问错误地址寄存器。 */
#define    HI_SC_STAT55_OFFSET                               (0x6DC) /* glb桥m5口（reserved）读访问错误地址寄存器。 */
#define    HI_SC_STAT56_OFFSET                               (0x6E0) /* glb桥m6口（hifi）写访问错误地址寄存器。 */
#define    HI_SC_STAT57_OFFSET                               (0x6E4) /* glb桥m6口（hifi）读访问错误地址寄存器。 */
#define    HI_SC_STAT62_OFFSET                               (0x6F8) /* bbphy桥m1口（dw_axi_glb）写访问错误地址寄存器。 */
#define    HI_SC_STAT63_OFFSET                               (0x6FC) /* bbphy桥m1口（dw_axi_glb）读访问错误地址寄存器。 */
#define    HI_SC_STAT64_OFFSET                               (0x700) /* bbphy桥m2口（dsp0）写访问错误地址寄存器。 */
#define    HI_SC_STAT65_OFFSET                               (0x704) /* bbphy桥m2口（dsp0）读访问错误地址寄存器。 */
#define    HI_SC_STAT66_OFFSET                               (0x708) /* bbphy桥m3口（bbp）写访问错误地址寄存器。 */
#define    HI_SC_STAT67_OFFSET                               (0x70C) /* bbphy桥m3口（bbp）读访问错误地址寄存器。 */
#define    HI_SC_STAT68_OFFSET                               (0x710) /* bbphy桥m4口（bbp）写访问错误地址寄存器。 */
#define    HI_SC_STAT69_OFFSET                               (0x714) /* bbphy桥m4口（bbp）读访问错误地址寄存器。 */
#define    HI_PWR_CTRL2_OFFSET                               (0xC08) /* TCXO控制寄存器 */
#define    HI_PWR_CTRL4_OFFSET                               (0xC10) /* ISOCELL隔离使能控制寄存器 */
#define    HI_PWR_CTRL5_OFFSET                               (0xC14) /* ISOCELL隔离关闭控制寄存器 */
#define    HI_PWR_CTRL6_OFFSET                               (0xC18) /* MTCMOS上电使能控制寄存器 */
#define    HI_PWR_CTRL7_OFFSET                               (0xC1C) /* MTCMOS下电使能控制寄存器 */
#define    HI_PWR_STAT1_OFFSET                               (0xE04) /* MTCMOS_RDY状态指示寄存器 */
#define    HI_PWR_STAT3_OFFSET                               (0xE0C) /* ISOCELL隔离控制状态寄存器 */
#define    HI_PWR_STAT4_OFFSET                               (0xE10) /* MTCMOS上下电控制状态寄存器 */
#define    HI_SEC_CTRL0_OFFSET                               (0xF04) /* 安全控制寄存器0(slave接口） */
#define    HI_SEC_CTRL1_OFFSET                               (0xF08) /* 安全控制寄存器1(GLB_MASTER接口） */
#define    HI_SEC_CTRL2_OFFSET                               (0xF0C) /* 安全控制寄存器2 */
#define    HI_SEC_CTRL3_OFFSET                               (0xF10) /* 安全控制寄存器3 */
#define    HI_SEC_CTRL4_OFFSET                               (0xF14) /* 安全控制寄存器4 */
#define    HI_SEC_CTRL5_OFFSET                               (0xF18) /* 安全控制寄存器5 */


#ifndef __ASSEMBLY__

/********************************************************************************/
/*    syssc 寄存器定义（项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 24; /* [23..0] 保留 */
        unsigned int    bbe_refclk_en              : 1; /* [24..24] BBE16输入参考时钟使能控制（给AP侧使用)。1 - 使能；0 - 无影响 */
        unsigned int    a9_refclk_en               : 1; /* [25..25] A9和总线输入参考时钟使能控制（给AP侧使用）。1 - 使能；0 - 无影响 */
        unsigned int    hpm_clk_en                 : 1; /* [26..26] HPM时钟使能控制。1 - 使能；0 - 无影响 */
        unsigned int    reserved_0                 : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKEN3_T;    /* 时钟使能3 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 24; /* [23..0] 保留 */
        unsigned int    bbe_refclk_dis             : 1; /* [24..24] BBE16输入参考时钟关闭控制（给AP侧使用）。1 - 使能；0 - 无影响 */
        unsigned int    a9_refclk_dis              : 1; /* [25..25] A9和总线输入参考时钟关闭控制（给AP侧使用）。1 - 使能；0 - 无影响 */
        unsigned int    hpm_clk_dis                : 1; /* [26..26] HPM时钟关闭控制。1 - 使能；0 - 无影响 */
        unsigned int    reserved_0                 : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKDIS3_T;    /* 时钟关闭3 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 24; /* [23..0] 保留 */
        unsigned int    bbe_refclk_status          : 1; /* [24..24] BBE16输入参考时钟状态控制（给AP侧使用）。1 - 使能；0 - 关闭(default) */
        unsigned int    a9_refclk_status           : 1; /* [25..25] A9和总线输入参考时钟状态控制（给AP侧使用）。1 - 使能；0 - 关闭(default) */
        unsigned int    hpm_clk_status             : 1; /* [26..26] HPM时钟状态控制。1 - 使能；0 - 关闭(default) */
        unsigned int    reserved_0                 : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKSTAT3_T;    /* 时钟状态3 */

typedef union
{
    struct
    {
        unsigned int    bbe_pd_clk_en              : 1; /* [0..0] BBE16 DSP子系统时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    bbe_core_clk_en            : 1; /* [1..1] BBE16 DSP子系统CORE时钟使能控制。0：无影响；1：时钟使能该bit只控制DSP CORE时钟。 */
        unsigned int    reserved_2                 : 1; /* [2..2] 保留 */
        unsigned int    mcpu_pd_clk_en             : 1; /* [3..3] ModemA9电源域时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    l2c_clk_en                 : 1; /* [4..4] L2Cache时钟使能控制。0：无影响；1：时钟使能 该bit同时控制送给AP侧时钟。 */
        unsigned int    reserved_1                 : 1; /* [5..5] 保留 */
        unsigned int    mcpu_mbist_clk_en          : 1; /* [6..6] ModemA9 MemoryBist时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    reserved_0                 : 1; /* [7..7] 保留。 */
        unsigned int    cicom0_clk_en              : 1; /* [8..8] CICOM0时钟使能。0：无影响；1：时钟使能 */
        unsigned int    cicom1_clk_en              : 1; /* [9..9] CICOM1时钟使能。1：时钟使能 0：无影响； */
        unsigned int    ipf_clk_en                 : 1; /* [10..10] IPF时钟使能控制。1：时钟使能 0：无影响； */
        unsigned int    upacc_clk_en               : 1; /* [11..11] UPACC时钟使能。1：时钟使能 0：无影响； */
        unsigned int    sci0_clk_en                : 1; /* [12..12] SCI0时钟使能控制（给AP侧使用）。1：时钟使能 0：无影响； */
        unsigned int    sci1_clk_en                : 1; /* [13..13] SCI1时钟使能控制（给AP侧使用）。0：无影响；1：时钟使能 */
        unsigned int    uicc_clk_en                : 1; /* [14..14] UICC模块时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    uart_clk_en                : 1; /* [15..15] UART工作时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    bbe_vic_clk_en             : 1; /* [16..16] BBE VIC中断控制器时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    cipher_clk_en              : 1; /* [17..17] CIPHER中断控制器时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    edmac_clk_en               : 1; /* [18..18] EDMAC时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    ipcm_clk_en                : 1; /* [19..19] IPCM时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    hsuart_clk_en              : 1; /* [20..20] HSUART时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    aximem_clk_en              : 1; /* [21..21] on-chip memory时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    aximon_clk_en              : 1; /* [22..22] AXI monitor时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    timer_clk_en               : 8; /* [30..23] TIMER0-7时钟使能控制。0：无影响；1：时钟使能 */
        unsigned int    wdog_clk_en                : 1; /* [31..31] WDOG时钟使能控制。0：无影响；1：时钟使能。 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKEN4_T;    /* 时钟使能4 */

typedef union
{
    struct
    {
        unsigned int    dsp0_pd_clk_dis            : 1; /* [0..0] DSP子系统时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    dsp0_core_clk_dis          : 1; /* [1..1] DSP子系统CORE时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    dsp0_dbg_clk_dis           : 1; /* [2..2] DSP子系统DBG时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    mcpu_pd_clk_dis            : 1; /* [3..3] ModemA9电源域时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    l2c_clk_dis                : 1; /* [4..4] L2Cache时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    mcpu_dbg_clk_dis           : 1; /* [5..5] ModemA9 Debug时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    mcpu_mbist_clk_dis         : 1; /* [6..6] ModemA9 MemoryBist时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    reserved                   : 1; /* [7..7] 保留 */
        unsigned int    cicom0_clk_dis             : 1; /* [8..8] CICOM0时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    cicom1_clk_dis             : 1; /* [9..9] CICOM1时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    ipf_clk_dis                : 1; /* [10..10] IPF时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    upacc_clk_dis              : 1; /* [11..11] UPACC时钟关闭控制。0：无影响；1：时钟关闭 */
        unsigned int    sci0_clk_dis               : 1; /* [12..12] SCI0时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    sci1_clk_dis               : 1; /* [13..13] SCI1时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    uicc_clk_dis               : 1; /* [14..14] UICC模块时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    uart_clk_dis               : 1; /* [15..15] UART工作时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    bbe_vic_clk_dis            : 1; /* [16..16] BBE VIC中断控制器时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    cipher_clk_dis             : 1; /* [17..17] CIPHER中断控制器时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    edmac_clk_dis              : 1; /* [18..18] EDMAC时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    ipcm_clk_dis               : 1; /* [19..19] IPCM时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    hsuart_clk_dis             : 1; /* [20..20] HSUART时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    aximem_clk_dis             : 1; /* [21..21] on-chip memory时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    aximon_clk_dis             : 1; /* [22..22] AXI monitor时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    timer_clk_dis              : 8; /* [30..23] TIMER0-7时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
        unsigned int    wdog_clk_dis               : 1; /* [31..31] WDOG时钟关闭控制。                            0：无影响；                                  1：时钟关闭 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKDIS4_T;    /* 时钟关闭4 */

typedef union
{
    struct
    {
        unsigned int    bbe_pd_clk_status          : 1; /* [0..0] BBEDSP子系统时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    bbe_core_clk_status        : 1; /* [1..1] BBEDSP子系统CORE时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    bbe_dbg_clk_status         : 1; /* [2..2] BBEDSP子系统DBG时钟控制状态。0：时钟关闭；1：时钟使能 该bit未使用。 */
        unsigned int    mcpu_pd_clk_status         : 1; /* [3..3] ModemA9电源域时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    l2c_clk_status             : 1; /* [4..4] L2Cache时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    mcpu_dbg_clk_status        : 1; /* [5..5] ModemA9 Debug时钟控制状态。0：时钟关闭；1：时钟使能 该bit未使用。 */
        unsigned int    mcpu_mbist_clk_status      : 1; /* [6..6] ModemA9 MemoryBist时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    reserved                   : 1; /* [7..7] 保留 */
        unsigned int    cicom0_clk_status          : 1; /* [8..8] CICOM0时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    cicom1_clk_status          : 1; /* [9..9] CICOM1时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    ipf_clk_status             : 1; /* [10..10] IPF时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    upacc_clk_status           : 1; /* [11..11] UPACC时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    sci0_clk_status            : 1; /* [12..12] SCI0时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    sci1_clk_status            : 1; /* [13..13] SCI1时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    uicc_clk_status            : 1; /* [14..14] UICC模块时钟状态。0：时钟关闭；1：时钟使能 */
        unsigned int    uart_clk_status            : 1; /* [15..15] UART工作时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    bbe_vic_clk_status         : 1; /* [16..16] BBE VIC中断控制器时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    cipher_clk_status          : 1; /* [17..17] CIPHER控制器时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    edmac_clk_status           : 1; /* [18..18] EDMAC时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    ipcm_clk_status            : 1; /* [19..19] IPCM时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    hsuart_clk_status          : 1; /* [20..20] HSUART时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    aximem_clk_status          : 1; /* [21..21] on-chip memory时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    aximon_clk_status          : 1; /* [22..22] AXI monitor时钟使控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    timer_clk_status           : 8; /* [30..23] TIMER0-7时钟控制状态。0：时钟关闭；1：时钟使能 */
        unsigned int    wdog_clk_status            : 1; /* [31..31] WDOG时钟控制状态。0：时钟关闭；1：时钟使能 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKSTAT4_T;    /* 时钟控制状态4 */

typedef union
{
    struct
    {
        unsigned int    bbp_ao_clk_en              : 1; /* [0..0] BBP AO区时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    bbpcommon_2a_clk_en        : 1; /* [1..1] BBPCOMMON模块时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    ltebbp_pd_clk_en           : 1; /* [2..2] LTE BBP掉电区时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    g1bbp_pd_clk_en            : 1; /* [3..3] G1 BBP掉电区时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    g2bbp_pd_clk_en            : 1; /* [4..4] G2 BBP掉电区时钟时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    twbbp_pd_clk_en            : 1; /* [5..5] TW BBP掉电区时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    wbbp_pd_clk_en             : 1; /* [6..6] WBBP掉电区时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    irm_pd_clk_en              : 1; /* [7..7] IRM BBP掉电区时钟时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    lbbp_axi_clk_en            : 1; /* [8..8] LTE BBP掉电区AXI接口时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    lbbp_pdt_clk_en            : 1; /* [9..9] LTE BBP掉电区时域部分时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    lbbp_pdf_clk_en            : 1; /* [10..10] LTE BBP掉电区频域部分时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    lbbp_tdl_clk_en            : 1; /* [11..11] LTE BBP掉电区TDL相关时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    lbbp_vdl_clk_en            : 1; /* [12..12] LTE BBP掉电区VDL相关时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    lbbp_tds_clk_en            : 1; /* [13..13] LTE BBP掉电区频域部分时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    reserved_1                 : 2; /* [15..14] 保留 */
        unsigned int    irm_turbo_clk_en           : 1; /* [16..16] IRM BBP掉电区 turbo时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    irm_bbc_245m_clk_en        : 1; /* [17..17] IRM BBP掉电区 245MHz时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    irm_wbbp_122m_clk_en       : 1; /* [18..18] IRM BBP掉电122MHz时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    twbbp_wbbp_122m_clk_en     : 1; /* [19..19] WTDS/W BBP掉电区TDS模122MHz时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    twbbp_wbbp_61m_clk_en      : 1; /* [20..20] TDS/W BBP掉电区 WBBP 61MHz时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    twbbp_tbbp_122m_clk_en     : 1; /* [21..21] PDTW BBP掉电区tbbp 122MHz时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    twbbp_tbbp_245m_clk_en     : 1; /* [22..22] PDTW BBP掉电区tbbp 245MHz时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    wbbp_61m_clk_en            : 1; /* [23..23] PDW BBP掉电区61MHz时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    wbbp_122m_clk_en           : 1; /* [24..24] PDW BBP掉电区122MHz时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    wbbp_245m_clk_en           : 1; /* [25..25] PDW BBP掉电区245MHz时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    wbbp_axi_clk_en            : 1; /* [26..26] PDW BBP掉电区AXI时钟使能控制。1 - 时钟打开0 - 无影响 */
        unsigned int    reserved_0                 : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKEN5_T;    /* 时钟使能5（BBP） */

typedef union
{
    struct
    {
        unsigned int    bbp_ao_clk_dis             : 1; /* [0..0] BBP DRX模块时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    bbpcommon_2a_clk_dis       : 1; /* [1..1] BBPCOMMON_2A模块时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    lbbp_pd_clk_dis            : 1; /* [2..2] LTEBBP电源域时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    g1bbp_pd_clk_dis           : 1; /* [3..3] G1BBP电源域时钟使关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    g2bbp_pd_clk_dis           : 1; /* [4..4] G2BBP电源域时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    twbbp_pd_clk_dis           : 1; /* [5..5] TWBBP电源域时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    wbbp_pd_clk_dis            : 1; /* [6..6] WBBP电源域时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    irmbbp_pd_clk_dis          : 1; /* [7..7] IRMBBP电源域时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    lbbp_axi_clk_dis           : 1; /* [8..8] LTEBBP AXI时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    lbbp_pdt_clk_dis           : 1; /* [9..9] LTEBBP时域相关时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    lbbp_pdf_clk_dis           : 1; /* [10..10] LTEBBP频域相关时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    lbbp_tdl_clk_dis           : 1; /* [11..11] LTEBBP TDL相关时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    lbbp_vdl_clk_dis           : 1; /* [12..12] LTEBBP VDL相关时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    lbbp_tds_clk_dis           : 1; /* [13..13] LTEBBP TDS相关时钟使能控制。1 - 时钟使能；0 - 无影响。 */
        unsigned int    reserved_1                 : 2; /* [15..14] 保留 */
        unsigned int    irm_turbo_clk_dis          : 1; /* [16..16] IRM turbo相关时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    irm_bbc_245m_clk_dis       : 1; /* [17..17] IRM 245MHz相关时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    irm_wbbp_clk_dis           : 1; /* [18..18] IRM WBBP相关时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    twbbp_wbbp_122m_clk_dis    : 1; /* [19..19] TWBBP wbbp 122MHz时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    twbbp_wbbp_61m_clk_dis     : 1; /* [20..20] TWBBP wbbp 61MHz时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    twbbp_tbbp_122m_clk_dis    : 1; /* [21..21] TWBBP tbbp 122MHz时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    twbbp_tbbp_245m_clk_dis    : 1; /* [22..22] TWBBP tbbp 245MHz时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    wbbp_61m_clk_dis           : 1; /* [23..23] WBBP电源域61MHz时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    wbbp_122m_clk_dis          : 1; /* [24..24] WBBP电源域122MHz时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    wbbp_245m_clk_dis          : 1; /* [25..25] WBBP电源域245MHz时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    wbbp_axi_clk_dis           : 1; /* [26..26] WBBP电源域AXI时钟关闭控制。1 - 时钟关闭；0 - 无影响。 */
        unsigned int    reserved_0                 : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKDIS5_T;    /* 时钟关闭控制寄存器0(BBP) */

typedef union
{
    struct
    {
        unsigned int    bbp_ao_clk_stat            : 1; /* [0..0] BBP DRX模块时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    bbpcom_clk_stat            : 1; /* [1..1] BBPCOM_2A模块时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    lbbp_pd_clk_stat           : 1; /* [2..2] LTEBBP电源域时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    g1bbp_pd_clk_stat          : 1; /* [3..3] G1BBP电源域时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    g2bbp_pd_clk_stat          : 1; /* [4..4] G2BBP电源域时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    twbbp_pd_clk_stat          : 1; /* [5..5] TWBBP电源域时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    wbbp_pd_clk_stat           : 1; /* [6..6] WBBP电源域时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    irmbbp_pd_clk_stat         : 1; /* [7..7] IRMBBP电源域时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    lbbp_axi_clk_stat          : 1; /* [8..8] LTEBBP AXI时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    lbbp_pdt_clk_stat          : 1; /* [9..9] LTEBBP时域相关时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    lbbp_pdf_clk_stat          : 1; /* [10..10] LTEBBP频域相关时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    lbbp_tdl_clk_stat          : 1; /* [11..11] LTEBBP TDL相关时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    lbbp_vdl_clk_stat          : 1; /* [12..12] LTEBBP VDL相关时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    lbbp_tds_clk_stat          : 1; /* [13..13] LTEBBP TDS相关时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    reserved_1                 : 2; /* [15..14] 保留 */
        unsigned int    irm_turbo_clk_stat         : 1; /* [16..16] IRM turbo相关时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    irm_bbc_245m_clk_stat      : 1; /* [17..17] IRM 245MHz相关时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    irm_wbbp_clk_stat          : 1; /* [18..18] IRM WBBP相关时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    twbbp_wbbp_122m_clk_stat   : 1; /* [19..19] TWBBP wbbp 122MHz时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    twbbp_wbbp_61m_clk_stat    : 1; /* [20..20] TWBBP wbbp 61MHz时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    twbbp_tbbp_122m_clk_stat   : 1; /* [21..21] TWBBP tbbp 122MHz时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    twbbp_tbbp_245m_clk_stat   : 1; /* [22..22] TWBBP tbbp 245MHz时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    wbbp_61m_clk_stat          : 1; /* [23..23] WBBP电源域61MHz时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    wbbp_122m_clk_stat         : 1; /* [24..24] WBBP电源域122MHz时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    wbbp_245m_clk_stat         : 1; /* [25..25] WBBP电源域245MHz时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    wbbp_axi_clk_stat          : 1; /* [26..26] WBBP电源域AXI时钟状态指示。1 - 时钟使能；0 - 时钟关闭。 */
        unsigned int    reserved_0                 : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKSTAT5_T;    /* 时钟状态控制寄存器0(BBP) */

typedef union
{
    struct
    {
        unsigned int    bbe_pd_srst_en             : 1; /* [0..0] BBEDSP子系统软复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    bbe_core_srst_en           : 1; /* [1..1] BBEDSP core软复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    bbe_dbg_srst_en            : 1; /* [2..2] BBEDSP DBG软复位请求使能控制。0：无影响1：复位请求有效    DSP debug相关逻辑不受bit【0】复位请求控制，DSP下电时，需要配置该bit复位debug相关逻辑 */
        unsigned int    reserved_4                 : 3; /* [5..3] 保留 */
        unsigned int    moda9_cpu_srst_en          : 1; /* [6..6] MODA9 CPU CORE软复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    moda9_dbg_srst_en          : 1; /* [7..7] MODA9 Debug软复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    moda9_peri_srst_en         : 1; /* [8..8] MODA9 外设软复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    moda9_scu_srst_en          : 1; /* [9..9] MODA9 SCU软复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    moda9_ptm_srst_en          : 1; /* [10..10] MODA9 PTM软复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    moda9_wd_srst_en           : 1; /* [11..11] MODA9 WD软复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    reserved_3                 : 3; /* [14..12] 保留 */
        unsigned int    mdma9_pd_srst_en           : 1; /* [15..15] MDMA9上电复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    reserved_2                 : 10; /* [25..16] 保留 */
        unsigned int    uicc_srst_en               : 1; /* [26..26] UICC软复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    reserved_1                 : 2; /* [28..27] 保留 */
        unsigned int    amon_soc_srst_en           : 1; /* [29..29] SOC AMONITOR软复位请求使能控制。0：无影响1：复位请求有效 */
        unsigned int    reserved_0                 : 1; /* [30..30] 保留 */
        unsigned int    hpm_srst_en                : 1; /* [31..31] HPM软复位请求使能控制。0：无影响1：复位请求有效 */
    } bits;
    unsigned int    u32;
}HI_CRG_SRSTEN1_T;    /* 软复位请求使能1 */

typedef union
{
    struct
    {
        unsigned int    bbe_pd_srst_dis            : 1; /* [0..0] BBEDSP子系统软复位请求关闭控制。0：无影响；1：复位请求无效 */
        unsigned int    bbe_core_srst_dis          : 1; /* [1..1] BBEDSP core软复位请求使关闭控制。0：无影响；1：复位请求无效 */
        unsigned int    bbe_dbg_srst_dis           : 1; /* [2..2] BBEDSP DBG软复位请求关闭控制。0：无影响；1：复位请求无效DSP debug相关逻辑不受bit【0】复位请求控制，DSP下电时，需要配置该bit复位debug相关逻辑 */
        unsigned int    reserved_4                 : 3; /* [5..3] 保留 */
        unsigned int    moda9_core_srst_dis        : 1; /* [6..6] MODA9 CPU CORE软复位请求关闭控制。0：无影响；1：复位请求无效 */
        unsigned int    moda9_dbg_srst_dis         : 1; /* [7..7] MODA9 Debug软复位请求关闭控制。0：无影响；1：复位请求无效 */
        unsigned int    moda9_peri_srst_dis        : 1; /* [8..8] MODA9 外设软复位请求关闭控制。0：无影响；1：复位请求无效 */
        unsigned int    moda9_scu_srst_dis         : 1; /* [9..9] MODA9 SCU软复位请求关闭控制。0：无影响；1：复位请求无效 */
        unsigned int    moda9_ptm_srst_dis         : 1; /* [10..10] MODA9 PTM软复位请求关闭控制。0：无影响；1：复位请求无效 */
        unsigned int    moda9_wd_srst_dis          : 1; /* [11..11] MODA9 WD软复位请求关闭控制。0：无影响；1：复位请求无效 */
        unsigned int    reserved_3                 : 3; /* [14..12] 保留 */
        unsigned int    mdma9_pd_srst_dis          : 1; /* [15..15] MDMA9上电复位请求关闭控制。0：无影响1：复位请求无效 */
        unsigned int    reserved_2                 : 10; /* [25..16] 保留 */
        unsigned int    uicc_srst_dis              : 1; /* [26..26] UICC软复位请求关闭控制。0：无影响1：复位请求无效 */
        unsigned int    reserved_1                 : 2; /* [28..27] 保留 */
        unsigned int    amon_soc_srst_dis          : 1; /* [29..29] SOC AMONITOR软复位请求关闭控制。0：无影响；1：复位请求无效 */
        unsigned int    reserved_0                 : 1; /* [30..30] 保留 */
        unsigned int    hpm_srst_dis               : 1; /* [31..31] HPMC软复位请求关闭控制。0：无影响1：复位请求无效 */
    } bits;
    unsigned int    u32;
}HI_CRG_SRSTDIS1_T;    /* 软复位请求关闭1 */

typedef union
{
    struct
    {
        unsigned int    bbe_pd_srst_status         : 1; /* [0..0] BBEDSP子系统软复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    bbe_core_srst_status       : 1; /* [1..1] BBEDSP core软复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    bbe_dbg_srst_status        : 1; /* [2..2] BBEDSP DBG软复位请求状态。0：复位请求无效；1：复位请求有效DSP debug相关逻辑不受bit【0】复位请求控制，DSP下电时，需要配置该bit复位debug相关逻辑 */
        unsigned int    reserved_4                 : 3; /* [5..3] 保留 */
        unsigned int    moda9_core_srst_status     : 1; /* [6..6] MODA9 CPU CORE软复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    moda9_dbg_srst_status      : 1; /* [7..7] MODA9 Debug软复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    moda9_peri_srst_status     : 1; /* [8..8] MODA9 外设软复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    moda9_scu_srst_status      : 1; /* [9..9] MODA9 SCU软复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    moda9_ptm_srst_status      : 1; /* [10..10] MODA9 PTM软复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    moda9_wd_srst_status       : 1; /* [11..11] MODA9 WD软复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    reserved_3                 : 3; /* [14..12] 保留 */
        unsigned int    mdma9_pd_srst_status       : 1; /* [15..15] MDMA9上电复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    reserved_2                 : 10; /* [25..16] 保留 */
        unsigned int    uicc_srst_status           : 1; /* [26..26] UICC软复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    reserved_1                 : 2; /* [28..27] 保留 */
        unsigned int    amon_soc_srst_status       : 1; /* [29..29] SOC AMONITOR软复位请求状态。0：复位请求无效；1：复位请求有效 */
        unsigned int    reserved_0                 : 1; /* [30..30] 保留 */
        unsigned int    hpm_srst_status            : 1; /* [31..31] HPM软复位请求状态。0：复位请求无效；1：复位请求有效 */
    } bits;
    unsigned int    u32;
}HI_CRG_SRSTSTAT1_T;    /* 软复位请求状态1 */

typedef union
{
    struct
    {
        unsigned int    bbp_crg_srst_en            : 1; /* [0..0] BBP时钟复位模块软复位请求使能控制。0 - 无影响1 - 复位有效 */
        unsigned int    bbpcommon_2a_srst_en       : 1; /* [1..1] BBPCOMMON_2A软复位请求使能控制。0 - 无影响1 - 复位有效 */
        unsigned int    lbbp_pd_srst_en            : 1; /* [2..2] LTEBBP整个掉电区掉电软复位请求使能控制。0 - 无影响1 - 复位有效 */
        unsigned int    g1bbp_pd_srst_en           : 1; /* [3..3] G1BBP掉电区掉电软复位请求使能控制。0 - 无影响1 - 复位有效 */
        unsigned int    g2bbp_pd_srst_en           : 1; /* [4..4] G2BBP掉电区掉电软复位请求使能控制。0 - 无影响1 - 复位有效 */
        unsigned int    twbbp_pd_srst_en           : 1; /* [5..5] TWBBP整个掉电区掉电软复位请求使能控制。0 - 无影响1 - 复位有效 */
        unsigned int    wbbp_pd_srst_en            : 1; /* [6..6] WBBP整个掉电区掉电软复位请求使能控制。0 - 无影响1 - 复位有效 */
        unsigned int    irm_pd_srst_en             : 1; /* [7..7] IRM掉电区掉电软复位请求使能控制。0 - 无影响1 - 复位有效 */
        unsigned int    bbc_lbbp_pdt_srst_en       : 1; /* [8..8] BBPCOMMON_2A LBBP PDT软复位源控制。0 - 无影响1 - 该bit置1 */
        unsigned int    bbc_lbbp_pdf_srst_en       : 1; /* [9..9] BBPCOMMON_2A LBBP PDF软复位源控制。0 - 无影响1 - 该bit置1 */
        unsigned int    bbc_lbbp_tdl_srst_en       : 1; /* [10..10] BBPCOMMON_2A LBBP TDL软复位源控制。0 - 无影响1 - 该bit置1 */
        unsigned int    bbc_tbbp_245m_srst_en      : 1; /* [11..11] BBPCOMMON_2A TBBP 245MHz部分逻辑软复位源控制。0 - 无影响1 - 该bit置1 */
        unsigned int    bbc_twbbp_122m_srst_en     : 1; /* [12..12] BBPCOMMON_2A TWBBP 122MHz部分逻辑软复位源控制。0 - 无影响1 - 该bit置1 */
        unsigned int    bbc_g1bbp_104m_srst_en     : 1; /* [13..13] BBPCOMMON_2A G1BBP 104MHz部分逻辑软复位源控制。0 - 无影响1 - 该bit置1 */
        unsigned int    bbc_g2bbp_104m_srst_en     : 1; /* [14..14] BBPCOMMON_2A G2BBP 104MHz部分逻辑软复位源控制。0 - 无影响1 - 该bit置1 */
        unsigned int    reserved                   : 16; /* [30..15] 保留。 */
        unsigned int    abb_srst_en                : 1; /* [31..31] ABB复位请求使能控制。0 - 无影响1 - 复位有效 */
    } bits;
    unsigned int    u32;
}HI_CRG_SRSTEN3_T;    /* 软复位请求使能3（ABB+BBP） */

typedef union
{
    struct
    {
        unsigned int    bbp_crg_srst_dis           : 1; /* [0..0] BBP时钟复位模块软复位请求关闭控制。0 - 无影响1 - 复位无效。 */
        unsigned int    bbpcommon_2a_pd_srst_dis   : 1; /* [1..1] BBPCOMMON_2A软复位请求关闭控制。0 - 无影响1 - 复位无效。 */
        unsigned int    lbbp_pd_srst_dis           : 1; /* [2..2] LTEBBP整个掉电区掉电软复位请求关闭控制。0 - 无影响1 - 复位无效。 */
        unsigned int    g1bbp_pd_srst_dis          : 1; /* [3..3] G1BBP掉电区掉电软复位请求关闭控制。0 - 无影响1 - 复位无效。 */
        unsigned int    g2bbp_pd_srst_dis          : 1; /* [4..4] G2BBP掉电区掉电软复位请求关闭控制。0 - 无影响1 - 复位无效。 */
        unsigned int    twbbp_pd_srst_dis          : 1; /* [5..5] TWBBP整个掉电区掉电软复位请求关闭控制。0 - 无影响1 - 复位无效。 */
        unsigned int    wbbp_pd_srst_dis           : 1; /* [6..6] WBBP整个掉电区掉电软复位请求关闭控制。0 - 无影响1 - 复位无效。 */
        unsigned int    irm_pd_srst_dis            : 1; /* [7..7] IRM掉电区掉电软复位请求关闭控制。0 - 无影响1 - 复位无效。 */
        unsigned int    bbc_lbbp_pdt_srst_dis      : 1; /* [8..8] BBPCOMMON_2A LBBP PDT软复位控制。0 - 无影响1 - 该bit清0 */
        unsigned int    bbc_lbbp_pdf_srst_dis      : 1; /* [9..9] BBPCOMMON_2A LBBP PDF软复位请求软复位控制。0 - 无影响1 - 该bit清0 */
        unsigned int    bbc_lbbp_tdl_srst_dis      : 1; /* [10..10] BBPCOMMON_2A LBBP TDL软复位请求软复位控制。0 - 无影响1 - 该bit清0 */
        unsigned int    bbc_tbbp_245m_srst_dis     : 1; /* [11..11] BBPCOMMON_2A TBBP 245MHz部分逻辑软复位控制。0 - 无影响1 - 该bit清0 */
        unsigned int    bbc_twbbp_122m_srst_dis    : 1; /* [12..12] BBPCOMMON_2A TWBBP 122MHz部分逻辑软复位控制。0 - 无影响1 - 该bit清0 */
        unsigned int    bbc_g1bbp_104m_srst_dis    : 1; /* [13..13] BBPCOMMON_2A G1BBP 104MHz部分逻辑软复位控制。0 - 无影响1 - 该bit清0 */
        unsigned int    bbc_g2bbp_104m_srst_dis    : 1; /* [14..14] BBPCOMMON_2A G2BBP 104MHz部分逻辑软复位控制。0 - 无影响1 - 该bit清0 */
        unsigned int    reserved                   : 16; /* [30..15] 保留 */
        unsigned int    abb_srst_dis               : 1; /* [31..31] ABB复位请求关闭控制。0 - 无影响1 - 复位有效 */
    } bits;
    unsigned int    u32;
}HI_CRG_SRSTDIS3_T;    /* 软复位请求关闭3（ABB+BBP） */

typedef union
{
    struct
    {
        unsigned int    bbp_crg_srst_stat          : 1; /* [0..0] BBP时钟复位模块软复位请求状态控制。0 - 复位无效1 - 复位有效 */
        unsigned int    bbpcommon_2a_srst_stat     : 1; /* [1..1] BBPCOMMON_2A软复位请求状态控制。0 - 复位无效1 - 复位有效 */
        unsigned int    lbbp_pd_srst_stat          : 1; /* [2..2] LTEBBP整个掉电区掉电软复位请求状态控制。0 - 复位无效1 - 复位有效 */
        unsigned int    g1bbp_pd_srst_stat         : 1; /* [3..3] G1BBP掉电区掉电软复位请求状态控制。0 - 复位无效1 - 复位有效 */
        unsigned int    g2bbp_pd_srst_stat         : 1; /* [4..4] G2BBP掉电区掉电软复位请求状态控制。0 - 复位无效1 - 复位有效 */
        unsigned int    twbbp_pd_srst_stat         : 1; /* [5..5] TWBBP整个掉电区掉电软复位请求状态控制。0 - 复位无效1 - 复位有效 */
        unsigned int    wbbp_pd_srst_stat          : 1; /* [6..6] WBBP整个掉电区掉电软复位请求状态控制。0 - 复位无效1 - 复位有效 */
        unsigned int    irm_pd_srst_stat           : 1; /* [7..7] IRM掉电区掉电软复位请求状态控制。0 - 复位无效1 - 复位有效 */
        unsigned int    bbc_lbbp_pdt_srstctrl_stat : 1; /* [8..8] BBPCOMMON_2A LBBP PDT软复位请求控制。0 - 复位受LTEBBP电压域软复位控制；1 - 复位受BBPCOMMON_2A软复位控制。 */
        unsigned int    bbc_lbbp_pdf_srstctrl_stat : 1; /* [9..9] BBPCOMMON_2A LBBP PDF软复位请求软复位控制。0 - 复位受LTEBBP电压域软复位控制；1 - 复位受BBPCOMMON_2A软复位控制。 */
        unsigned int    bbc_lbbp_tdl_srstctrl_stat : 1; /* [10..10] BBPCOMMON_2A LBBP TDL软复位请求软复位请求控制。0 - 复位受LTEBBP电压域软复位控制；1 - 复位受BBPCOMMON_2A软复位控制。 */
        unsigned int    bbc_tbbp_245m_srstctrl_stat : 1; /* [11..11] BBPCOMMON_2A TBBP 245MHz部分逻辑软复位请求控制。0 - 复位受TWEBBP电压域软复位控制；1 - 复位受BBPCOMMON_2A软复位控制。 */
        unsigned int    bbc_twbbp_122m_srstctrl_stat : 1; /* [12..12] BBPCOMMON_2A TWBBP 122MHz部分逻辑软复位请求控制。0 - 复位受TWEBBP电压域软复位控制；1 - 复位受BBPCOMMON_2A软复位控制。 */
        unsigned int    bbc_g1bbp_104m_srstctrl_stat : 1; /* [13..13] BBPCOMMON_2A G1BBP 104MHz部分逻辑软复位请求控制。0 - 复位受G1EBBP电压域软复位控制；1 - 复位受BBPCOMMON_2A软复位控制。 */
        unsigned int    bbc_g2bbp_104m_srstctrl_stat : 1; /* [14..14] BBPCOMMON_2A G2BBP 104MHz部分逻辑软复位软复位请求控制。0 - 复位受G2EBBP电压域软复位控制；1 - 复位受BBPCOMMON_2A软复位控制。 */
        unsigned int    reserved                   : 16; /* [30..15] 保留 */
        unsigned int    abb_srst_status            : 1; /* [31..31] ABB复位请求状态控制。0 - 复位无效1 - 复位有效 */
    } bits;
    unsigned int    u32;
}HI_CRG_SRSTSTAT3_T;    /* 软复位请求状态3（ABB+BBP） */

typedef union
{
    struct
    {
        unsigned int    reserved_4                 : 4; /* [3..0] 保留 */
        unsigned int    mdma9_clk_div              : 4; /* [7..4] modem A9分频系数。只要满足慢速总线及快速总线的频率比，可以是0-15的任何值；modem A9最高666MHz，即A9_PLL postdiv时钟被此分频系数分频后的频率。默认A9PLL POSTDIV输出时钟频率为400MHz */
        unsigned int    a92slow_freqmode           : 3; /* [10..8] modem A9 CORE时钟与慢速总线频率比配置(1/N)。只要满足和A9及慢速总线的频率比，可以是1-8的任何值； */
        unsigned int    reserved_3                 : 1; /* [11..11] 保留 */
        unsigned int    a92fast_freqmode           : 3; /* [14..12] modem A9 CORE时钟与L2C总线频率比配置(1/N)。只要满足和A9及快速总线的频率比，可以是1-8的任何值； */
        unsigned int    reserved_2                 : 1; /* [15..15] 保留 */
        unsigned int    a9_clksw_req               : 3; /* [18..16] A9和总线时钟切换请求。只能有1bits为1.bit[0] - 选择A9PLL输出时钟；bit[1] - 选择DSPPLL输出时钟；bit[2] - 选择19，2MHz时钟(default) */
        unsigned int    reserved_1                 : 11; /* [29..19] 保留 */
        unsigned int    apb_freqmode               : 1; /* [30..30] APB总线与SlowBus总线分频比配置。0 - 1:11 - 2:1(default) */
        unsigned int    reserved_0                 : 1; /* [31..31] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKDIV2_T;    /* 时钟分频控制2(总线分频） */

typedef union
{
    struct
    {
        unsigned int    bbe_clk_div_num            : 2; /* [1..0] BBE16 CORE时钟分频系数。最高工作频率为400MHz。0 - 1分频N - N+1分频 */
        unsigned int    reserved_2                 : 2; /* [3..2] 保留 */
        unsigned int    bbe_freqmode               : 2; /* [5..4] DSP CORE和总线分频比。总线的最高工作频率为200MHz.0 - 1:11 - 2:1(default)10 -3:111- 4:1 */
        unsigned int    reserved_1                 : 22; /* [27..6] 保留 */
        unsigned int    cipher_clk_div             : 3; /* [30..28] cipher工作时钟分频系数。分频器的输入参考时钟为BBPPLL，491.52MHz。N：N分频(N=3~7)；（默认配置为3分频）cipher_clk=491.53MHz/3=163MHz. */
        unsigned int    reserved_0                 : 1; /* [31..31] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLKDIV5_T;    /* 时钟分频控制5(BBEDSP/CIPHER时钟分频) */

typedef union
{
    struct
    {
        unsigned int    timer_clk_sel              : 8; /* [7..0] TIMER0-7工作时钟源选择，静态切换。1 - 选择19.2MHz时钟；0 - 选择32KHz时钟Note：切换时钟时，最好先关闭对应的时钟输出，然后切换时钟，切换完毕后打开时钟 */
        unsigned int    gps_tcxo_clk_sel           : 1; /* [8..8] GPS TCXO时钟选择。静态切换0 - TCXO0 CLK1 - TCXO1 CLK */
        unsigned int    hpm_refclk_sel             : 1; /* [9..9] HPM工作时钟选择。0 - 选择a9_refclk;1 - 选择dsp_refclk。 */
        unsigned int    reserved_1                 : 10; /* [19..10] 保留 */
        unsigned int    sc_a9wdt_rst_en            : 1; /* [20..20] A9 WDT复位请求有效。1 - A9子系统内的WDT复位有效时，复位A90 - A9子系统内的WDT复位有效时，不复位A9 */
        unsigned int    reserved_0                 : 11; /* [31..21] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLK_SEL2_T;    /* 时钟源选择控制2(TIMER) */

typedef union
{
    struct
    {
        unsigned int    abb_tcxo_clk_sel           : 1; /* [0..0] ABB TCXO时钟源选择。0 - TCXO0时钟；1 - TCXO1时钟 */
        unsigned int    bbppll_refclk_sel          : 1; /* [1..1] BBPPLL参考时钟源选择。静态切换。0 - ABB通道0 TCXO时钟；1 - ABB通道1 TCXO时钟 */
        unsigned int    bbp_tcxo_clk_sel           : 1; /* [2..2] 保留。 */
        unsigned int    ch0_tcxo_clk_sel           : 1; /* [3..3] 通道0 19.2MHz时钟选择。静态切换。0 - ABB通道0 TCXO时钟；1 - ABB通道1 TCXO时钟 */
        unsigned int    ch1_tcxo_clk_sel           : 1; /* [4..4] 通道1 19.2MHz时钟选择。静态切换。0 - ABB通道0 TCXO时钟；1 - ABB通道1 TCXO时钟 */
        unsigned int    g1bbp_104m_clk_sel         : 1; /* [5..5] G1BBP通道选择。静态切换。0 - 选择通道0；1 - 选择通道1 */
        unsigned int    g2bbp_104m_clk_sel         : 1; /* [6..6] G2BBP通道选择。静态切换。0 - 选择通道0；1 - 选择通道1 */
        unsigned int    tdl_clk_sel                : 1; /* [7..7] TDL时钟源选择。0 - 122MHz/245MHz；1 - 245MHz/491MHz */
        unsigned int    lbbp_vdl_clk_sel           : 1; /* [8..8] VDL时钟源选择。0 - 122MHz；1 - 245MHz */
        unsigned int    lbbp_pdf_clk_sel           : 1; /* [9..9] LTEBBP 频域时钟源选择。0 - 122MHz；1 - 245MHz */
        unsigned int    reserved                   : 22; /* [31..10] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_CLK_SEL3_T;    /* 时钟源选择控制3（BBP） */

typedef union
{
    struct
    {
        unsigned int    pll_en                     : 1; /* [0..0] DFS工作模式。1：正常工作模式；0：Power Down模式。 */
        unsigned int    pll_bp                     : 1; /* [1..1] DFS旁路控制。0：正常操作fOUT=fOUT；1：旁路fOUT=fREF。 */
        unsigned int    pll_refdiv                 : 6; /* [7..2] 频率合成器输出参考时钟分频系数。 */
        unsigned int    pll_intprog                : 12; /* [19..8] 频率合成器倍频整数部分。 */
        unsigned int    pll_postdiv1               : 3; /* [22..20] VCO一级分频系数。 */
        unsigned int    pll_postdiv2               : 3; /* [25..23] VCO二级分频系数。 */
        unsigned int    pll_lock                   : 1; /* [26..26] DFS锁定指示，高有效。0：失锁；1：锁定。 */
        unsigned int    reserved                   : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_A9PLL_CFG0_T;    /* A9PLL配置寄存器0 */

typedef union
{
    struct
    {
        unsigned int    pll_fracdiv                : 24; /* [23..0] 频率合成器倍频小数部分。 */
        unsigned int    pll_int_mod                : 1; /* [24..24] 分数分频工作模式。0：使能分数分频；1：禁止分数分频。 */
        unsigned int    pll_cfg_vld                : 1; /* [25..25] PLL配置有效标志0 - 配置无效；1 - 配置有效 */
        unsigned int    pll_clk_gt                 : 1; /* [26..26] PLL输出时钟门控信号。0 - PLL输出时钟门控。1 - PLL输出时钟不门控。该bit需要PLL锁定后配置。 */
        unsigned int    reserved                   : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_A9PLL_CFG1_T;    /* A9PLL配置寄存器1 */

typedef union
{
    struct
    {
        unsigned int    pll_ssc_reset              : 1; /* [0..0] 复位有效，高电平有效。0 - 复位撤离；1 - 复位有效。该bit不能在PLL有效时配置，否则会导致PLL失锁。 */
        unsigned int    pll_ssc_disable            : 1; /* [1..1] Bypass the modulator,高电平有效。0 - 正常工作；1 - bypass */
        unsigned int    pll_ssc_downspread         : 1; /* [2..2] 0 - center spread1 - down spread */
        unsigned int    pll_ssc_spread             : 3; /* [5..3] 设置调制深度(spread%).000 - 0001 - 0.049%^010 - 0.098%011 - 0.195%100 - 0.391%101 - 0.781%110 - 1.563%111 - 3.125% */
        unsigned int    pll_ssc_divval             : 4; /* [9..6] 设置频谱调制的频率，一版约为32KHzfreq=(CLKSSCG/(DIVCAL* #points),其中points=128;CLKSSCG=FREF(REFDIV=1时） */
        unsigned int    reserved                   : 22; /* [31..10] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_A9PLL_CFG2_T;    /* A9PLL配置寄存器2 */

typedef union
{
    struct
    {
        unsigned int    pll_en                     : 1; /* [0..0] DFS工作模式。1：正常工作模式；0：Power Down模式。 */
        unsigned int    pll_bp                     : 1; /* [1..1] DFS旁路控制。0：正常操作fOUT=fOUT；1：旁路fOUT=fREF。 */
        unsigned int    pll_refdiv                 : 6; /* [7..2] 频率合成器输出参考时钟分频系数。 */
        unsigned int    pll_intprog                : 12; /* [19..8] 频率合成器倍频整数部分。 */
        unsigned int    pll_postdiv1               : 3; /* [22..20] VCO二级分频系数。 */
        unsigned int    pll_postdiv2               : 3; /* [25..23] VCO二级分频系数。 */
        unsigned int    pll_lock                   : 1; /* [26..26] DFS锁定指示，高有效。0：失锁；1：锁定。 */
        unsigned int    reserved                   : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_DSPPLL_CFG0_T;    /* BBEPLL配置寄存器0 */

typedef union
{
    struct
    {
        unsigned int    pll_fracdiv                : 24; /* [23..0] 频率合成器倍频小数部分。 */
        unsigned int    pll_int_mod                : 1; /* [24..24] 分数分频工作模式。0：使能分数分频；1：禁止分数分频。 */
        unsigned int    pll_cfg_vld                : 1; /* [25..25] PLL配置有效标志0 - 配置无效；1 - 配置有效 */
        unsigned int    pll_clk_gt                 : 1; /* [26..26] PLL输出时钟门控信号。0 - PLL输出时钟门控。1 - PLL输出时钟不门控。该bit需要PLL锁定后配置。 */
        unsigned int    reserved                   : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_DSPPLL_CFG1_T;    /* BBEPLL配置寄存器1 */

typedef union
{
    struct
    {
        unsigned int    pll_ssc_reset              : 1; /* [0..0] 复位有效，高电平有效。0 - 复位撤离；1 - 复位有效。该bit不能在PLL有效时配置，否则会导致PLL失锁。 */
        unsigned int    pll_ssc_disable            : 1; /* [1..1] Bypass the modulator,高电平有效。0 - 正常工作；1 - bypass */
        unsigned int    pll_ssc_downspread         : 1; /* [2..2] 0 - center spread1 - down spread */
        unsigned int    pll_ssc_spread             : 3; /* [5..3] 设置调制深度(spread%).000 - 0001 - 0.049%^010 - 0.098%011 - 0.195%100 - 0.391%101 - 0.781%110 - 1.563%111 - 3.125% */
        unsigned int    pll_ssc_divval             : 4; /* [9..6] 设置频谱调制的频率，一版约为32KHzfreq=(CLKSSCG/(DIVCAL* #points),其中points=128;CLKSSCG=FREF(REFDIV=1时） */
        unsigned int    reserved                   : 22; /* [31..10] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_DSPPLL_CFG2_T;    /* BBEPLL配置寄存器2 */

typedef union
{
    struct
    {
        unsigned int    pll_en                     : 1; /* [0..0] DFS工作模式。1：正常工作模式；0：Power Down模式。 */
        unsigned int    pll_bp                     : 1; /* [1..1] DFS旁路控制。0：正常操作fOUT=fOUT；1：旁路fOUT=fREF。 */
        unsigned int    pll_refdiv                 : 6; /* [7..2] 频率合成器输出参考时钟分频系数。 */
        unsigned int    pll_intprog                : 12; /* [19..8] 频率合成器倍频整数部分。 */
        unsigned int    pll_postdiv1               : 3; /* [22..20] VCO二级分频系数。 */
        unsigned int    pll_postdiv2               : 3; /* [25..23] VCO二级分频系数。 */
        unsigned int    pll_lock                   : 1; /* [26..26] DFS锁定指示，高有效。0：失锁；1：锁定。 */
        unsigned int    reserved                   : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_BBPPLL_CFG0_T;    /* BBPPLL配置寄存器0 */

typedef union
{
    struct
    {
        unsigned int    pll_fracdiv                : 24; /* [23..0] 频率合成器倍频小数部分。 */
        unsigned int    pll_int_mod                : 1; /* [24..24] 分数分频工作模式。0：使能分数分频；1：禁止分数分频。 */
        unsigned int    pll_cfg_vld                : 1; /* [25..25] PLL配置有效标志0 - 配置无效；1 - 配置有效 */
        unsigned int    pll_clk_gt                 : 1; /* [26..26] PLL输出时钟门控信号。0 - PLL输出时钟门控。1 - PLL输出时钟不门控。该bit需要PLL锁定后配置。 */
        unsigned int    reserved                   : 5; /* [31..27] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_BBPPLL_CFG1_T;    /* BBPPLL配置寄存器1 */

typedef union
{
    struct
    {
        unsigned int    pll_ssc_reset              : 1; /* [0..0] 复位有效，高电平有效。0 - 复位撤离；1 - 复位有效。该bit不能在PLL有效时配置，否则会导致PLL失锁。 */
        unsigned int    pll_ssc_disable            : 1; /* [1..1] Bypass the modulator,高电平有效。0 - 正常工作；1 - bypass */
        unsigned int    pll_ssc_downspread         : 1; /* [2..2] 0 - center spread1 - down spread */
        unsigned int    pll_ssc_spread             : 3; /* [5..3] 设置调制深度(spread%).000 - 0001 - 0.049%^010 - 0.098%011 - 0.195%100 - 0.391%101 - 0.781%110 - 1.563%111 - 3.125% */
        unsigned int    pll_ssc_divval             : 4; /* [9..6] 设置频谱调制的频率，一版约为32KHzfreq=(CLKSSCG/(DIVCAL* #points),其中points=128;CLKSSCG=FREF(REFDIV=1时） */
        unsigned int    reserved                   : 22; /* [31..10] 保留 */
    } bits;
    unsigned int    u32;
}HI_CRG_BBPPLL_CFG2_T;    /* BBPPLL配置寄存器2 */

typedef union
{
    struct
    {
        unsigned int    mcpu_boot_remap_clear      : 1; /* [0..0] ModemA9启动地址重映射控制。0 - remap，A9的0地址重映射到配置的启动地址1 - 0地址不重映射该bit可以一直保持为0，此时ModemA9访问不到低64KB的DDR空间。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL0_T;    /* ModemA9启动Remap控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    wdt_clk_en                 : 1; /* [0..0] 看门狗时钟使能控制。0：不使能；1：使能（默认值）。注意：对该位进行写0操作时，需要先将该寄存器高16位值写为0x6920（即将wdt_en_ctrl位写为0x6920）打开保护，然后再对此比特写数值才可以有效，避免误操作。 */
        unsigned int    reserved_1                 : 2; /* [2..1] 保留（数据可写入）。 */
        unsigned int    wdt_en_ov                  : 1; /* [3..3] WDT时钟强制使能控制位。0：不使能（默认值）；1：使能。当wdt_en_ov使能时，看门狗以总线时钟（pclk）计数，否则以32K时钟计数。该位一般情况下，不需要使用，一直配置为0。 */
        unsigned int    reserved_0                 : 12; /* [15..4] 保留（数据可写入）。 */
        unsigned int    wdt_en_ctrl                : 16; /* [31..16] wdt_en写使能控制。该控制位是为了防止软件误操作看门狗而设立的保护逻辑，看门狗时钟使能开关只有在该位配置为6920后，才可更改。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL2_T;    /* 看门狗配置寄存器。 */

typedef union
{
    struct
    {
        unsigned int    reserved_3                 : 10; /* [9..0] 保留 */
        unsigned int    amon_monitor_start         : 1; /* [10..10] 监控Global AXI总线axi monitor启动停止信号。0：停止；1：启动。 */
        unsigned int    reserved_2                 : 4; /* [14..11] 保留。 */
        unsigned int    dsp0_uart_en               : 2; /* [16..15] LTEDSP UART选择bit16：选择UART0控制。bit15：选择hs uart控制。每比特值为1表示使用对应uart，0表示不使用 */
        unsigned int    reserved_1                 : 14; /* [30..17] 保留。 */
        unsigned int    reserved_0                 : 1; /* [31..31] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL3_T;    /* 外设配置寄存器。 */

typedef union
{
    struct
    {
        unsigned int    cicom0_sel_mod             : 1; /* [0..0] 模式选择位。0：WCDMA加解密模式（默认值）；1：GSM加解密模式。 */
        unsigned int    cicom1_sel_mod             : 1; /* [1..1] 模式选择位。0：WCDMA加解密模式（默认值）；1：GSM加解密模式。 */
        unsigned int    reserved                   : 30; /* [31..2] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL4_T;    /* CICOM加解密模式配置寄存器。 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 32; /* [31..0] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL5_T;    /* 保留寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 32; /* [31..0] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL6_T;    /* 保留寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 16; /* [15..0] 保留。 */
        unsigned int    sw_dsp0_boot_addr          : 16; /* [31..16] BBE16外部启动地址配置。地址空间大小64KB，该地址需要64KB对齐。如果采用外部启动方式，需要根据实际启动地址配置该寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL9_T;    /* BBE16启动地址配置寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL10_T;    /* 保留寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_2                 : 1; /* [0..0] 保留。 */
        unsigned int    moda9_cfgnmfi              : 1; /* [1..1] 快速中断不可屏蔽控制位。0：清除；1：设置（默认值）。只在CPU reset时被采样。 */
        unsigned int    moda9_cfgsdisable          : 1; /* [2..2] 处理器部分重要配置寄存器写保护。0：不使能；1：使能。该比特使能后可以防止distributor、A9处理器接口、LSPIs的部分寄存器被修改。如果需要使用此功能，ARM建议用户在boot阶段软件配置完成相关寄存器后使能该功能。理论上，系统只有在硬复位时需要关闭该保护功能。 */
        unsigned int    moda9_cp15sdisable         : 1; /* [3..3] secure register写保护。0：不使能；1：使能。reset 0， then 1 by software。保护CPU内部寄存器设置，由系统寄存器逻辑保护此值只可改变一次。 */
        unsigned int    moda9_pwrctli0             : 2; /* [5..4] CPU0状态寄存器[1:0]复位值。如无电源管理，必须接0。 */
        unsigned int    reserved_1                 : 10; /* [15..6] 保留 */
        unsigned int    moda9_l2_waysize           : 3; /* [18..16] moda9 L2 cache way size。000：reserved001：16KB010：32KB(default)others：reserved */
        unsigned int    reserved_0                 : 1; /* [19..19] 保留。 */
        unsigned int    moda9_l2_regfilebase       : 12; /* [31..20] moda9 L2 cache 寄存器基地址。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL11_T;    /* Modem A9配置寄存器。 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL12_T;    /* 保留寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 16; /* [15..0] 保留。 */
        unsigned int    ocdhaltonreset             : 1; /* [16..16] BBE16在解复位时强行进入OCDHalt模式。0：正常模式（默认值）；1：OCDHalt模式。该bit在解复位前6个周期及后10个周期不能修改。在解复位完成后，改变该bit没有影响。 */
        unsigned int    runstall                   : 1; /* [17..17] BBE16运行/停止。0：运行；1：停止。（默认值） */
        unsigned int    statvectorsel              : 1; /* [18..18] BBE16启动地址选择0：内部TCM启动（默认值）1：外部启动，启动地址由外部启动地址寄存器配置 */
        unsigned int    breaksync_en               : 3; /* [21..19]  */
        unsigned int    crosstrig_en               : 3; /* [24..22]  */
        unsigned int    reserved_0                 : 7; /* [31..25] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL13_T;    /* BBE16配置寄存器 */

typedef union
{
    struct
    {
        unsigned int    hpm_clk_div                : 6; /* [5..0] HPM clock dividor */
        unsigned int    reserved_1                 : 1; /* [6..6] 保留 */
        unsigned int    hpm_en                     : 1; /* [7..7] HPM使能控制。1 - 使能。 */
        unsigned int    hpmx_en                    : 1; /* [8..8] HPMX使能控制。1 - 使能。 */
        unsigned int    reserved_0                 : 23; /* [31..9] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL17_T;    /* HPM控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    axi_mem_gatedclock_en      : 1; /* [0..0] axi_mem自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    cicom0_auto_clk_gate_en    : 1; /* [1..1] cicom0自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    cicom0_soft_gate_clk_en    : 1; /* [2..2] cicom0时钟门控控制(在自动门控使能为0时有效)。0：不关断（默认值）；1：关断时钟。 */
        unsigned int    cicom1_auto_clk_gate_en    : 1; /* [3..3] cicom1自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    cicom1_soft_gate_clk_en    : 1; /* [4..4] cicom1时钟门控控制(在自动门控使能为0时有效)。0：不关断（默认值）；1：关断时钟。 */
        unsigned int    hs_uart_gatedclock_en      : 1; /* [5..5] hs_uart自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    uart_gatedclock_en         : 1; /* [6..6] 非掉电区uart0, 掉电区uart1/2/3自动门控使能1：使能0：不使能（默认值） */
        unsigned int    reserved_2                 : 12; /* [18..7] 保留。 */
        unsigned int    uicc_gatedclock_en         : 1; /* [19..19] UICC时钟门控控制(在自动门控使能为0时有效)。0：不关断（默认值）；1：关断时钟。 */
        unsigned int    uicc_ss_scaledown_mode     : 2; /* [21..20] uicc仿真加速控制。00：加速关闭（默认值）；01：使能除speed enumeration,HNP/SRP,Host mode suspend nad resume之外的timingvalue；10：仅使能device mode suspend and resume timing value；11：使能比特0、1 scale-down timing values。综合时必须为00。 */
        unsigned int    upacc_auto_clk_gate_en     : 1; /* [22..22] upacc自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    upacc_soft_gate_clk_en     : 1; /* [23..23] upacc时钟门控控制(在自动门控使能为0时有效)。0：不关断（默认值）；1：关断时钟。 */
        unsigned int    bbe16_cg_en                : 1; /* [24..24] dsp0 ram自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    reserved_1                 : 3; /* [27..25] 保留。 */
        unsigned int    ipcm_auto_clk_gate_en      : 1; /* [28..28] ipcm自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    ipcm_soft_gate_clk_en      : 1; /* [29..29] ipcm时钟门控控制(在自动门控使能为0时有效)。0：不关断（默认值）；1：关断时钟。 */
        unsigned int    reserved_0                 : 1; /* [30..30] 保留。 */
        unsigned int    sysreg_auto_gate_en        : 1; /* [31..31] 系统控制器配置接口时钟自动门控使能控制。1 - 自动门控使能。0 -不使能。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL20_T;    /* 自动门控时钟控制寄存器0 */

typedef union
{
    struct
    {
        unsigned int    reserved_6                 : 4; /* [3..0] 保留。 */
        unsigned int    timer_gatedclock_en        : 1; /* [4..4] timer0-7自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    reserved_5                 : 5; /* [9..5] 保留。 */
        unsigned int    dw_axi_glb_cg_en           : 1; /* [10..10] global axi自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    dw_axi_mst_cg_en           : 1; /* [11..11] master axi自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    reserved_4                 : 1; /* [12..12] 保留。 */
        unsigned int    dw_axi_bbphy_cg_en         : 1; /* [13..13] bbphy axi自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    reserved_3                 : 1; /* [14..14] 保留 */
        unsigned int    dw_x2x_async_cg_en         : 1; /* [15..15] async x2x自动门控使能。0：不使能（默认值）；1：使能。async x2x分布在：DSP0两个和l2c接口 */
        unsigned int    reserved_2                 : 1; /* [16..16] 保留 */
        unsigned int    dw_x2h_qsync_cg_en         : 1; /* [17..17] rs axi自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    dw_hmx_cg_en               : 1; /* [18..18] hmx自动门控使能。0：不使能（默认值）；1：使能。位于ahb2axi_mst里各一个 */
        unsigned int    dw_x2p_cg_en               : 1; /* [19..19] x2p自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    dw_gs_cg_en                : 1; /* [20..20] gs自动门控使能。0：不使能（默认值）；1：使能。gs位于axi mem里 */
        unsigned int    ashb_gatedclock_en         : 1; /* [21..21] h2x自动门控使能。0：不使能（默认值）；1：使能。位于m3和ahb2axi_mst里各一个 */
        unsigned int    dw_ahb_mst_gatedclock_en   : 1; /* [22..22] mst ahb自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    reserved_1                 : 1; /* [23..23] 保留。 */
        unsigned int    edmac_autogated_clk_en     : 1; /* [24..24] edmac自动门控使能。0：不使能（默认值）；1：使能。 */
        unsigned int    reserved_0                 : 7; /* [31..25] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL21_T;    /* 自动门控时钟控制寄存器1 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 16; /* [15..0] 保留。 */
        unsigned int    bbe16_ema                  : 3; /* [18..16] extra margin adjustment ,[0] = LSB */
        unsigned int    bbe16_emaw                 : 2; /* [20..19] extra margin adjustment write,[0] = LSB */
        unsigned int    bbe16_emas                 : 1; /* [21..21] extra margin adjustment sense amplifier pulse */
        unsigned int    reserved_0                 : 10; /* [31..22] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL22_T;    /* BBE16 memory 时序配置寄存器 */

typedef union
{
    struct
    {
        unsigned int    axi_mem_ema                : 3; /* [2..0] extra margin adjustment ,[0] = LSB */
        unsigned int    axi_mem_emaw               : 2; /* [4..3] extra margin adjustment write,[0] = LSB */
        unsigned int    axi_mem_emas               : 1; /* [5..5] extra margin adjustment sense amplifier pulse */
        unsigned int    reserved_1                 : 2; /* [7..6] 保留。 */
        unsigned int    soc_mem_ema                : 3; /* [10..8] extra margin adjustment ,[0] = LSB；控制soc spram ，dpram，以及tpram的A口。对soc_mem的注解：soc spram包括：hs_uart，ipf,cipher,uicc模块内的spram；soc tpram包括：edmac，amon，ipf，cipher模块内的tpram； */
        unsigned int    soc_mem_spram_emaw         : 2; /* [12..11] extra margin adjustment write,[0] = LSB */
        unsigned int    soc_mem_spram_emas         : 1; /* [13..13] extra margin adjustment sense amplifier pulse */
        unsigned int    soc_mem_tpram_emab         : 3; /* [16..14] extra margin adjustment ,[0] = LSB；控制soc tpram的B口。 */
        unsigned int    soc_mem_tpram_emasa        : 1; /* [17..17] extra margin adjustment sense amplifier pulse */
        unsigned int    soc_mem_dpram_emaw         : 2; /* [19..18] extra margin adjustment write,[0] = LSB */
        unsigned int    soc_mem_dpram_emas         : 1; /* [20..20] extra margin adjustment sense amplifier pulse */
        unsigned int    reserved_0                 : 11; /* [31..21] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL23_T;    /* SOC memory 时序配置寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 10; /* [9..0] 保留。 */
        unsigned int    moda9_hs_mem_adjust        : 10; /* [19..10] ModA9 memory 时序/低功耗控制。[9:7]：保留[6]：ret1n, default=1'b0[5]：EMAS, default-1'b1[4:3]：EMAW, default=2'b01[2:0]：EMA, default=3'b011 */
        unsigned int    reserved_0                 : 6; /* [25..20] 保留。 */
        unsigned int    moda9_l2_ema               : 3; /* [28..26] extra margin adjustment ,[0] = LSB */
        unsigned int    moda9_l2_emaw              : 2; /* [30..29] extra margin adjustment write,[0] = LSB */
        unsigned int    moda9_l2_emas              : 1; /* [31..31] extra margin adjustment sense amplifier pulse */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL24_T;    /* memory 时序配置寄存器2（CCPU+L2)。 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 32; /* [31..0] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL25_T;    /* SOC 测试信号选择寄存器(保留） */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL26_T;    /* SOC 测试信号bypass寄存器(保留） */

typedef union
{
    struct
    {
        unsigned int    acp_filter_start           : 32; /* [31..0] 地址过滤首地址。即C段起始地址为0x5000_0000。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL28_T;    /* 地址过滤首地址(ACP空间起始地址) */

typedef union
{
    struct
    {
        unsigned int    acp_filter_end             : 32; /* [31..0] 地址过滤尾地址。即C段结束地址为0x7FEF_FFFF。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL29_T;    /* 地址过滤尾地址(ACP空间结束地址) */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 32; /* [31..0] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL30_T;    /* 保留寄存器 */

typedef union
{
    struct
    {
        unsigned int    uicc_ic_usb_vbusvalid      : 1; /* [0..0] VBUS有效控制。0：无效；1：有效。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL45_T;    /* uicc控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp0_nmi                   : 16; /* [15..0] dsp0不可屏蔽中断，任一bit为高即有效。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL52_T;    /* BBE16不可屏蔽中断寄存器。 */

typedef union
{
    struct
    {
        unsigned int    reserved_7                 : 5; /* [4..0] 保留 */
        unsigned int    uicc_ret1n                 : 1; /* [5..5] memory retention mode1控制。0：进入retention；1：不进入retention,Normal工作模式。 */
        unsigned int    reserved_6                 : 3; /* [8..6] 保留 */
        unsigned int    amon_soc_ret1n             : 1; /* [9..9] memory retention mode1控制。0：进入retention；1：不进入retention,Normal工作模式。 */
        unsigned int    reserved_5                 : 3; /* [12..10] 保留 */
        unsigned int    edmac_colldisn             : 1; /* [13..13] memory读写冲突电路disable控制。0：读写冲突电路不使能；1：读写冲突电路使能(default)。 */
        unsigned int    reserved_4                 : 3; /* [16..14] 保留 */
        unsigned int    amon_soc_colldisn          : 1; /* [17..17] memory读写冲突电路disable控制。0：disable 读写冲突电路；1：enable 读写冲突电路。 */
        unsigned int    reserved_3                 : 1; /* [18..18] 保留 */
        unsigned int    ipf_ret1n                  : 1; /* [19..19] memory retention mode1控制。0：进入retention；1：不进入retention。 */
        unsigned int    ipf_colldisn               : 1; /* [20..20] memory读写冲突电路disable控制。0：读写冲突电路不使能；1：读写冲突电路使能(default)。 */
        unsigned int    cipher_ret1n               : 1; /* [21..21] memory retention mode1控制。0：进入retention；1：不进入retention。 */
        unsigned int    cipher_colldisn            : 1; /* [22..22] memory读写冲突电路disable控制。0：读写冲突电路不使能；1：读写冲突电路使能(default)。 */
        unsigned int    reserved_2                 : 2; /* [24..23] 保留 */
        unsigned int    hs_uart_ret1n              : 1; /* [25..25] memory retention mode1控制。0：进入retention；1：不进入retention。 */
        unsigned int    reserved_1                 : 3; /* [28..26] 保留 */
        unsigned int    edmac_ret1n                : 1; /* [29..29] memory retention mode1控制。0：进入retention；1：不进入retention。 */
        unsigned int    reserved_0                 : 2; /* [31..30] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL55_T;    /* memory低功耗配置寄存器0 */

typedef union
{
    struct
    {
        unsigned int    prior_level                : 2; /* [1..0] 总线优先级设置。00：优先级dma > axi;01：优先级axi > dma. */
        unsigned int    gatedclock_en              : 1; /* [2..2] lbus总线自动时钟门控使能。1：使能；0：禁止。 */
        unsigned int    dw_axi_rs_gatedclock_en    : 1; /* [3..3] axi rs自动门控使能。1：使能；0：禁止。 */
        unsigned int    dw_axi_gs_gatedclock_en    : 1; /* [4..4] axi gs自动门控使能。1：使能；0：禁止。 */
        unsigned int    overf_prot                 : 2; /* [6..5] 总线防挂死保护选项。00：关闭防挂死保护；01：使能防挂死保护，超时时间：512 cycles；10：使能防挂死保护，超时时间：1024 cycles；11：使能防挂死保护，超时时间：2048 cycles */
        unsigned int    reserved                   : 9; /* [15..7] 保留。 */
        unsigned int    atpram_ctrl                : 16; /* [31..16] memory低功耗控制。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL56_T;    /* BBP LBUS控制寄存器。 */

typedef union
{
    struct
    {
        unsigned int    soc2lte_tbd                : 16; /* [15..0] BBP保留寄存器 */
        unsigned int    reserved                   : 16; /* [31..16] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL57_T;    /* BBP保留控制寄存器。 */

typedef union
{
    struct
    {
        unsigned int    moda9_l2_ret1n             : 1; /* [0..0] memory retention mode1控制。0：进入retention；1：不进入retention。 */
        unsigned int    moda9_l2_ret2n             : 1; /* [1..1] memory retention mode2控制。0：进入retention；1：不进入retention。该bit未使用。 */
        unsigned int    moda9_l2_pgen              : 1; /* [2..2] memory power down控制。0：不进入power down；1：进入power down。该bit未使用。 */
        unsigned int    reserved_2                 : 5; /* [7..3] 保留。 */
        unsigned int    dsp0_ret1n                 : 1; /* [8..8] memory retention mode1控制。0：进入retention；1：不进入retention。 */
        unsigned int    dsp0_ret2n                 : 1; /* [9..9] memory retention mode2控制。V7R2中memory不支持此bit控制0：进入retention；1：不进入retention。该bit未使用。 */
        unsigned int    dsp0_pgen                  : 1; /* [10..10] memory power down控制。V7R2中memory不支持此bit控制0：不进入power down；1：进入power down。该bit未使用。 */
        unsigned int    reserved_1                 : 5; /* [15..11] 保留。 */
        unsigned int    pd_axi_mem_ret1n           : 1; /* [16..16] memory retention mode1控制。0：进入retention；1：不进入retention。 */
        unsigned int    pd_axi_mem_ret2n           : 1; /* [17..17] memory retention mode2控制。0：进入retention；1：不进入retention。该bit未使用。 */
        unsigned int    pd_axi_mem_pgen            : 1; /* [18..18] memory power down控制。0：不进入power down；1：进入power down。该bit未使用。 */
        unsigned int    reserved_0                 : 13; /* [31..19] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL68_T;    /* memory低功耗配置寄存器2 */

typedef union
{
    struct
    {
        unsigned int    axi_bbphy_xdcdr_sel        : 1; /* [0..0] AXI_BBPHY桥译码逻辑控制。0：访问关钟、复位、掉电模块仅报错不改变译码逻辑，不返回DECERR；1：访问关钟、复位、掉电模块报错且译码到default slave（防挂死逻辑使能，SC_CTRL103对应bit配置为0)，返回DECERR； */
        unsigned int    reserved_2                 : 2; /* [2..1] 保留 */
        unsigned int    axi_glb_xdcdr_sel          : 1; /* [3..3] AXI_GLB桥译码逻辑控制。0：访问关钟、复位、掉电模块仅报错不改变译码逻辑，不返回DECERR；1：访问关钟、复位、掉电模块报错且译码到default slave（防挂死逻辑使能，SC_CTRL103对应bit配置为0)，返回DECERR； */
        unsigned int    reserved_1                 : 1; /* [4..4] 保留 */
        unsigned int    socapb_pslverr_sel         : 1; /* [5..5] APB外设译码逻辑控制。0：访问关钟、复位、掉电模块仅报错不改变译码逻辑，不返回DECERR；1：访问关钟、复位、掉电模块报错且译码到default slave（防挂死逻辑使能，SC_CTRL69对应bit配置为0)，返回DECERR； */
        unsigned int    x2h_hslverr_sel            : 1; /* [6..6] x2h桥译码逻辑：0：访问关钟、复位、掉电模块仅报错不改变译码逻辑，系统可能挂死；1：访问关钟、复位、掉电模块报错且译码到default slave（防挂死逻辑使能，SC_CTRL105对应bit配置为0)，系统不挂死，default slave返回ok； */
        unsigned int    mst_err_srst_req           : 1; /* [7..7] 所有错误信息软复位请求。0：不复位。1：复位所有mst_err中断和状态。 */
        unsigned int    reserved_0                 : 24; /* [31..8] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL69_T;    /* 系统错误地址访问功能寄存器 */

typedef union
{
    struct
    {
        unsigned int    axi_bbphy_priority_m1      : 2; /* [1..0] dw_axi_bbphy m1口（dw_axi_glb桥）优先级配置。值越大优先级越高。 */
        unsigned int    axi_bbphy_priority_m2      : 2; /* [3..2] dw_axi_bbphy m2口（dsp0）优先级配置。值越大优先级越高。 */
        unsigned int    axi_bbphy_priority_m3      : 2; /* [5..4] dw_axi_bbphy m3口（LTEBBP/TDSBBP）优先级配置。值越大优先级越高。 */
        unsigned int    axi_bbphy_priority_m4      : 2; /* [7..6] dw_axi_bbphy m4口（WBBP）优先级配置。 */
        unsigned int    reserved_1                 : 8; /* [15..8] dw_axi_guacc m4口（upacc）优先级配置。值越大优先级越高。相同值的端口号小的优先级高。 */
        unsigned int    axi_mst_priority_m9        : 4; /* [19..16] dw_axi_mxt m9口（保留）优先级配置。值越大优先级越高。 */
        unsigned int    axi_mst_priority_m10       : 4; /* [23..20] dw_axi_mxt m10口（保留）优先级配置。值越大优先级越高。 */
        unsigned int    reserved_0                 : 8; /* [31..24] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL70_T;    /* 互联优先级配置寄存器0 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 4; /* [3..0] 保留 */
        unsigned int    axi_glb_priority_m1        : 3; /* [6..4] dw_axi_glb m1口（dw_axi_bbphy）优先级配置。值越大优先级越高。 */
        unsigned int    axi_glb_priority_m2        : 3; /* [9..7] dw_axi_glb m2口（dw_axi_mst）优先级配置。值越大优先级越高。 */
        unsigned int    axi_glb_priority_m3        : 3; /* [12..10] dw_axi_glb m3口（AP侧）优先级配置。值越大优先级越高。 */
        unsigned int    axi_glb_priority_m4        : 3; /* [15..13] dw_axi_glb m4口（mdma9）优先级配置。值越大优先级越高。 */
        unsigned int    axi_glb_priority_m5        : 3; /* [18..16] dw_axi_glb m5口（WBBP）优先级配置。值越大优先级越高。 */
        unsigned int    axi_glb_priority_m6        : 3; /* [21..19] dw_axi_glb m6口（保留）优先级配置。值越大优先级越高。 */
        unsigned int    axi_mst_cache_cfg_en       : 1; /* [22..22] ModemCPU cache功能使能。0：使用外设master cache（默认）；1：使用axi_mst_cache配置值（地址在DDR空间） */
        unsigned int    axi_mst_sideband           : 5; /* [27..23] 由GLB AXI桥传入到ACP AW/ARUSERS端的控制信号。[27:24]内部属性0000：strongly-ordered；0001：device；0011：normal memory non-cacheable；0110：write-through；0111：write-back no write allocate；1111：write-back write allocate。[23]共享，AWIDMx[2]=0时0：non-coherent request；1：coherent request。 */
        unsigned int    axi_mst_cache              : 4; /* [31..28] MOD CPU cache值。0000：noncacheable，nonbufferable0001：bufferable only0010：cacheable，but do not allocate0011：cacheable，bufferable，but do not allocate0100：reserved0101：reserved0110：cachable write-through, allocate on reads only0111：cachable write-back, allocate on reads only1000：reserved1001：reserved1010：cachable write-through, allocate on writes only1011：cachable write-back, allocate on writes only1100：reserved1101：reserved1110：cachable write-through, allocate on write & reads 1111：cachable write-back, allocate on write &reads */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL71_T;    /* 互联优先级配置寄存器1 */

typedef union
{
    struct
    {
        unsigned int    axi_mst_priority_m1        : 4; /* [3..0] dw_axi_mxt m1口（dw_ahb-UICC）优先级配置。值越大优先级越高。 */
        unsigned int    axi_mst_priority_m2        : 4; /* [7..4] dw_axi_mxt m2口（edmac_ch16）优先级配置。值越大优先级越高。 */
        unsigned int    axi_mst_priority_m3        : 4; /* [11..8] dw_axi_mxt m3口（cipher）优先级配置。值越大优先级越高。 */
        unsigned int    axi_mst_priority_m4        : 4; /* [15..12] dw_axi_mxt m4口（ipf）优先级配置。值越大优先级越高。 */
        unsigned int    axi_mst_priority_m5        : 4; /* [19..16] dw_axi_mxt m5口（UPACC）优先级配置。值越大优先级越高。 */
        unsigned int    axi_mst_priority_m6        : 4; /* [23..20] dw_axi_mxt m6口（CICOM0）优先级配置。值越大优先级越高。 */
        unsigned int    axi_mst_priority_m7        : 4; /* [27..24] dw_axi_mxt m7口（CICOM1）优先级配置。值越大优先级越高。 */
        unsigned int    axi_mst_priority_m8        : 4; /* [31..28] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL72_T;    /* 互联优先级配置寄存器2（DW_axi_mst) */

typedef union
{
    struct
    {
        unsigned int    peri_mst_region1_ctrl      : 8; /* [7..0] REGION1访问控制，每个bit表示一个Master。Bit[0] - UICCBit[1] - EDMACBit[2] - CIPHERBit[3] - IPFBit[4] - UPACCBit[5] - CICOM0Bit[6] - CICOM1Bit[7] - 保留。每个bit含义：1 - 该Master可以访问该地址空间。0 - 该master不可以访问该地址空间。 */
        unsigned int    peri_mst_secctrl_bypass    : 1; /* [8..8] 0 - 加速器外设可以访问所有空间(default)1 - 外设加速器的访问空间受控，具体参见peri_mst_regionN_ctrl的配置 */
        unsigned int    reserved                   : 1; /* [9..9] 保留。 */
        unsigned int    peri_mst_region1_staddr    : 22; /* [31..10] REGION1空间起始地址，以1KB为单位。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL73_T;    /* 加速器访问空间控制寄存器0 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 10; /* [9..0] 保留。 */
        unsigned int    peri_mst_region1_endaddr   : 22; /* [31..10] REGION1空间结束地址，以1KB为单位。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL74_T;    /* 加速器访问空间控制寄存器1 */

typedef union
{
    struct
    {
        unsigned int    peri_mst_region2_ctrl      : 8; /* [7..0] REGION1访问控制，每个bit表示一个Master。Bit[0] - UICCBit[1] - EDMACBit[2] - CIPHERBit[3] - IPFBit[4] - UPACCBit[5] - CICOM0Bit[6] - CICOM1Bit[7] - 保留。每个bit含义：1 - 该Master可以访问该地址空间。0 - 该master不可以访问该地址空间。 */
        unsigned int    reserved                   : 2; /* [9..8] 保留。 */
        unsigned int    peri_mst_region2_staddr    : 22; /* [31..10] REGION2空间起始地址，以1KB为单位。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL75_T;    /* 加速器访问空间控制寄存器2 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 10; /* [9..0] 保留。 */
        unsigned int    peri_mst_region2_endaddr   : 22; /* [31..10] REGION2空间结束地址，以1KB为单位。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL76_T;    /* 加速器访问空间控制寄存器3 */

typedef union
{
    struct
    {
        unsigned int    peri_mst_region3_ctrl      : 8; /* [7..0] REGION1访问控制，每个bit表示一个Master。Bit[0] - UICCBit[1] - EDMACBit[2] - CIPHERBit[3] - IPFBit[4] - UPACCBit[5] - CICOM0Bit[6] - CICOM1Bit[7] - 保留。每个bit含义：1 - 该Master可以访问该地址空间。0 - 该master不可以访问该地址空间。 */
        unsigned int    reserved                   : 2; /* [9..8] 保留。 */
        unsigned int    peri_mst_region3_staddr    : 22; /* [31..10] REGION3空间起始地址，以1KB为单位。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL77_T;    /* 加速器访问空间控制寄存器4 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 10; /* [9..0] 保留。 */
        unsigned int    peri_mst_region3_endaddr   : 22; /* [31..10] REGION3空间结束地址，以1KB为单位。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL78_T;    /* 加速器访问空间控制寄存器5 */

typedef union
{
    struct
    {
        unsigned int    peri_mst_region4_ctrl      : 8; /* [7..0] REGION1访问控制，每个bit表示一个Master。Bit[0] - UICCBit[1] - EDMACBit[2] - CIPHERBit[3] - IPFBit[4] - UPACCBit[5] - CICOM0Bit[6] - CICOM1Bit[7] - 保留。每个bit含义：1 - 该Master可以访问该地址空间。0 - 该master不可以访问该地址空间。 */
        unsigned int    reserved                   : 2; /* [9..8] 保留。 */
        unsigned int    peri_mst_region4_staddr    : 22; /* [31..10] REGION4空间起始地址，以1KB为单位。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL79_T;    /* 加速器访问空间控制寄存器6 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 10; /* [9..0] 保留。 */
        unsigned int    peri_mst_region4_endaddr   : 22; /* [31..10] REGION1空间结束地址，以1KB为单位。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL80_T;    /* 加速器访问空间控制寄存器7 */

typedef union
{
    struct
    {
        unsigned int    reserved_8                 : 1; /* [0..0] 保留。 */
        unsigned int    bbphy_dsp0_axislv_active_mask : 1; /* [1..1] dw_axi_bbyphy桥上dsp0 axi slave口active信号屏蔽控制。1 - 屏蔽使能0 - 屏蔽不使能。 */
        unsigned int    bbphy_bbp_axislv_active_mask : 1; /* [2..2] dw_axi_bbyphy桥上bbp axi slave口active信号屏蔽控制（LTEBBP/TWBBP/BBC_2A/BBC_2B)）。1 - 屏蔽使能0 - 屏蔽不使能。 */
        unsigned int    reserved_7                 : 1; /* [3..3] 保留。 */
        unsigned int    reserved_6                 : 4; /* [7..4] 保留。 */
        unsigned int    mdmacp_axislv_active_mask  : 1; /* [8..8] dw_axi_glb桥上mdma9 ACP slave口active信号屏蔽寄存器。 */
        unsigned int    reserved_5                 : 2; /* [10..9] 保留 */
        unsigned int    axi_mem_axislv_active_mask : 1; /* [11..11] dw_axi_glb桥上axi mem slave口active信号屏蔽控制。1 - 屏蔽使能0 - 屏蔽不使能。 */
        unsigned int    reserved_4                 : 1; /* [12..12] 保留 */
        unsigned int    bbphy_axislv_active_mask   : 1; /* [13..13] dw_axi_glb桥上bbephy桥 slave口active信号屏蔽控制。1 - 屏蔽使能0 - 屏蔽不使能。 */
        unsigned int    reserved_3                 : 1; /* [14..14] 保留 */
        unsigned int    wbbp_axislv_active_mask    : 1; /* [15..15] dw_axi_glb桥上WBBP/G1BBP/G2BBP slave口active信号屏蔽控制。1 - 屏蔽使能0 - 屏蔽不使能。 */
        unsigned int    amon_axislv_active_mask    : 1; /* [16..16] dw_axi_glb桥上amon_slave口active信号屏蔽控制。1 - 屏蔽使能0 - 屏蔽不使能。 */
        unsigned int    reserved_2                 : 2; /* [18..17] 保留。 */
        unsigned int    wdog_apbslv_active_mask    : 1; /* [19..19] wdog apb slave口active信号屏蔽控制。1 - 屏蔽使能0 - 屏蔽不使能。 */
        unsigned int    timer0_7_apbslv_active_mask : 1; /* [20..20] timer0_7 apb slave口active信号屏蔽控制。1 - 屏蔽使能0 - 屏蔽不使能。 */
        unsigned int    reserved_1                 : 4; /* [24..21] 保留。 */
        unsigned int    uart0_apbslv_active_mask   : 1; /* [25..25] uart0 apb slave口active信号屏蔽控制。1 - 屏蔽使能0 - 屏蔽不使能。 */
        unsigned int    edmac_slv_active_mask      : 1; /* [26..26] edma  apb slave口active信号屏蔽控制。1 - 屏蔽使能0 - 屏蔽不使能。 */
        unsigned int    reserved_0                 : 5; /* [31..27] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL103_T;    /* slave active屏蔽寄存器0 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 32; /* [31..0] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL104_T;    /* 保留寄存器 */

typedef union
{
    struct
    {
        unsigned int    uicc                       : 1; /* [0..0] uicc ahb slave口active信号屏蔽寄存器。0：不屏蔽；当处于不工作状态（关闭时钟，复位）时，访问该模块将返回error 或者上报中断，同时记录地址信息。1：屏蔽active逻辑；slave固定为可访问状态； */
        unsigned int    reserved_6                 : 5; /* [5..1] 保留 */
        unsigned int    ipcm                       : 1; /* [6..6] ipcm ahb slave口active信号屏蔽寄存器。0：不屏蔽；当处于不工作状态（关闭时钟，复位）时，访问该模块将返回error 或者上报中断，同时记录地址信息。1：屏蔽active逻辑；slave固定为可访问状态； */
        unsigned int    reserved_5                 : 1; /* [7..7] 保留 */
        unsigned int    hs_uart                    : 1; /* [8..8] hs uart ahb slave口active信号屏蔽寄存器。0：不屏蔽；当处于不工作状态（关闭时钟，复位）时，访问该模块将返回error 或者上报中断，同时记录地址信息。1：屏蔽active逻辑；slave固定为可访问状态； */
        unsigned int    reserved_4                 : 2; /* [10..9] 保留 */
        unsigned int    cipher                     : 1; /* [11..11] cipher ahb slave口active信号屏蔽寄存器。0：不屏蔽；当处于不工作状态（关闭时钟，复位）时，访问该模块将返回error 或者上报中断，同时记录地址信息。1：屏蔽active逻辑；slave固定为可访问状态； */
        unsigned int    ipf                        : 1; /* [12..12] ipf ahb slave口active信号屏蔽寄存器。0：不屏蔽；当处于不工作状态（关闭时钟，复位）时，访问该模块将返回error 或者上报中断。1：屏蔽active逻辑；slave固定为可访问状态； */
        unsigned int    reserved_3                 : 6; /* [18..13] 保留 */
        unsigned int    cicom0                     : 1; /* [19..19] cicom0 ahb slave口active信号屏蔽寄存器。0：不屏蔽；当处于不工作状态（关闭时钟，复位）时，访问该模块将返回error 或者上报中断。1：屏蔽active逻辑；slave固定为可访问状态； */
        unsigned int    cicom1                     : 1; /* [20..20] cicom1 ahb slave口active信号屏蔽寄存器。0：不屏蔽；当处于不工作状态（关闭时钟，复位）时，访问该模块将返回error 或者上报中断，同时记录地址信息。1：屏蔽active逻辑；slave固定为可访问状态； */
        unsigned int    reserved_2                 : 4; /* [24..21] 保留 */
        unsigned int    vic1                       : 1; /* [25..25] vic1 ahb slave口active信号屏蔽寄存器。0：不屏蔽；当处于不工作状态（关闭时钟，复位）时，访问该模块将返回error 或者上报中断，同时记录地址信息。1：屏蔽active逻辑；slave固定为可访问状态； */
        unsigned int    reserved_1                 : 3; /* [28..26] 保留 */
        unsigned int    upacc                      : 1; /* [29..29] upacc ahb slave口active信号屏蔽寄存器。0：不屏蔽；当处于不工作状态（关闭时钟，复位）时，访问该模块将返回error 或者上报中断，同时记录地址信息。1：屏蔽active逻辑；slave固定为可访问状态； */
        unsigned int    reserved_0                 : 2; /* [31..30] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_CTRL105_T;    /* slave active屏蔽寄存器2 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 14; /* [13..0] 保留。 */
        unsigned int    moda9_standbywfi           : 1; /* [14..14] ModA9 WFI状态指示。0：非WFI模式；1：进入WFI模式。 */
        unsigned int    moda9_standbywfe           : 1; /* [15..15] ModA9 WFE状态指示。0：非WFE模式；1：进入WFE模式。 */
        unsigned int    moda9_pmupriv              : 1; /* [16..16] ModA9状态指示。0：用户模式；1：特权模式。该比特信息在CoreSight中不提供。 */
        unsigned int    moda9_pmusecure            : 1; /* [17..17] ModA9安全状态。0：非安全状态；1：安全状态。该比特信息在CoreSight中不提供。 */
        unsigned int    moda9_smpnamp              : 1; /* [18..18] ModA9 AMP/SMP模式。0：不对称；1：对称。 */
        unsigned int    moda9_scuevabort           : 1; /* [19..19] 异常标志信号，指示在一致性逐出期间外部发生了终止操作，用于CPU性能监控，不关心下可以悬空。0：未发生1：已发生 */
        unsigned int    moda9_pwrctlo0             : 2; /* [21..20] CPU当前工作状态。0x：CPU0必须上电；10：CPU0可以进入dormant mode；11：CPU0可以进入powered-off mode。 */
        unsigned int    moda9_l2_tagclkouten       : 1; /* [22..22] l2 tagclkouten状态。 */
        unsigned int    moda9_l2_dataclkouten      : 4; /* [26..23] l2 dataclkouten状态。 */
        unsigned int    moda9_l2_idle              : 1; /* [27..27] l2 idle状态。 */
        unsigned int    moda9_l2_clkstopped        : 1; /* [28..28] l2 clk_stopped 状态。 */
        unsigned int    reserved_0                 : 3; /* [31..29] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT1_T;    /* Modem A9状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 16; /* [15..0] 保留。 */
        unsigned int    dsp0_pwaitmode             : 1; /* [16..16] 处理器睡眠状态指示。0：正常；1：睡眠。当DSP执行WAITI指令等待中断时，此信号置起。任何没有disable的中断都会唤醒处理器。 */
        unsigned int    dsp0_xocdmode              : 1; /* [17..17] 指示处理器处于OCD Halt模式。0：正常；1：OCD Halt模式。 */
        unsigned int    reserved_0                 : 14; /* [31..18] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT2_T;    /* BBE16 DSP状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    hpmx_opc                   : 10; /* [9..0] hpmx_opc的值 */
        unsigned int    hpmx_opc_vld               : 1; /* [10..10] hpm_opc有效指示信号。0：无效；1：有效。 */
        unsigned int    reserved_1                 : 1; /* [11..11] 保留 */
        unsigned int    hpm_opc                    : 10; /* [21..12] hpm_opc的值 */
        unsigned int    hpm_opc_vld                : 1; /* [22..22] hpm_opc有效指示信号。0：无效；1：有效。 */
        unsigned int    reserved_0                 : 9; /* [31..23] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT3_T;    /* HPM状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp0_nmi                   : 1; /* [0..0] DSP非屏蔽中断 */
        unsigned int    intr_ipc_ns                : 1; /* [1..1] AP_IPCM_NS给BBE16的快速中断 */
        unsigned int    intr_ipc_ns_mbx            : 1; /* [2..2] AP_IPCM_NS给BBE16的邮箱中断 */
        unsigned int    dsp_uart_int               : 1; /* [3..3] HS_UART和UART0的组合中断。 */
        unsigned int    edmac_int1                 : 1; /* [4..4] EDMAC中断1 */
        unsigned int    lte_dsp_aagc_int           : 1; /* [5..5] AAGC上报中断 */
        unsigned int    lte_dsp_cell_int           : 1; /* [6..6] 小区搜索中断 */
        unsigned int    lte_dsp_vdl_int            : 1; /* [7..7] viterbi译码链路中断 */
        unsigned int    lte_dsp_cmu_int            : 1; /* [8..8] 公共测量上报中断 */
        unsigned int    lte_dsp_pwrm_int           : 1; /* [9..9] 能量测量上报中断 */
        unsigned int    lte_dsp_cfi_int            : 1; /* [10..10] CFI的浮动中断 */
        unsigned int    lte_dsp_tdl_int            : 1; /* [11..11] Turbo译码链路中断 */
        unsigned int    lte_dsp_bbp_dma_int        : 1; /* [12..12] DMA快速通道完成通道 */
        unsigned int    lte_dsp_pub_int            : 1; /* [13..13] 合并中断 */
        unsigned int    lte_dsp_synb_int           : 1; /* [14..14] DSP符号中断 */
        unsigned int    tds_stu_sfrm_int           : 1; /* [15..15] STU子帧中断 */
        unsigned int    tds_hsupa_int              : 1; /* [16..16] TDS HSUPA完成中断 */
        unsigned int    tds_harq_int               : 1; /* [17..17] TDS HARQ完成中断 */
        unsigned int    tds_turbo_int              : 1; /* [18..18] TDS Turbo完成中断 */
        unsigned int    tds_viterbi_int            : 1; /* [19..19] TDS Viterbi完成中断 */
        unsigned int    tds_rfc_int                : 1; /* [20..20] 射频前端中断 */
        unsigned int    tds_fpu_int                : 1; /* [21..21] 前端中断 */
        unsigned int    tds_stu_dsp_int            : 1; /* [22..22] STU子帧中断和时隙中断 */
        unsigned int    ctu_int_lte                : 1; /* [23..23] LTE模CTU中断 */
        unsigned int    ctu_int_tds                : 1; /* [24..24] TDS模CTU中断 */
        unsigned int    dsp_vic_int                : 1; /* [25..25] DSP VIC中断 */
        unsigned int    reserved                   : 6; /* [31..26] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT5_T;    /* BBE16中断状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 12; /* [11..0] 保留。 */
        unsigned int    moda9_parityfail0          : 8; /* [19..12] A9 RAM阵列校验结果。0：校验成功；1：校验失败。[7] BTAC parity error[6] GHB[5] Instruction tag RAM[4] Instruction data RAM[3] Main TLB[2] D outer RAM[1] Data tag RAM[0] Data data RAM该版本不支持该功能，固定为0 */
        unsigned int    moda9_parityfailscu        : 1; /* [20..20] SCU tag RAM校验结果。0：校验成功；1：校验失败。该版本不支持该功能，固定为0 */
        unsigned int    reserved_0                 : 11; /* [31..21] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT6_T;    /* Modem A9校验状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    dw_axi_mst_dlock_wr        : 1; /* [0..0] 死锁传输读写指示。0：读；1：写。 */
        unsigned int    dw_axi_mst_dlock_slv       : 1; /* [1..1] 跟随死锁master的slave端口号。 */
        unsigned int    reserved_2                 : 2; /* [3..2] 保留。 */
        unsigned int    dw_axi_mst_dlock_mst       : 4; /* [7..4] 死锁的master最小端口号。 */
        unsigned int    reserved_1                 : 12; /* [19..8] 保留。 */
        unsigned int    dw_axi_glb_dlock_wr        : 1; /* [20..20] 死锁传输读写指示。0：读；1：写。 */
        unsigned int    dw_axi_glb_dlock_slv       : 4; /* [24..21] 跟随死锁master的slave端口号。 */
        unsigned int    dw_axi_glb_dlock_mst       : 3; /* [27..25] 死锁的master最小端口号。 */
        unsigned int    reserved_0                 : 4; /* [31..28] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT9_T;    /* AXI桥锁死状态寄存器0 */

typedef union
{
    struct
    {
        unsigned int    dw_axi_glb_dlock_id        : 8; /* [7..0] GLB AXI桥死锁ID号 */
        unsigned int    reserved                   : 20; /* [27..8] 保留。 */
        unsigned int    dw_axi_mst_dlock_id        : 4; /* [31..28] MST AXI桥死锁ID号 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT10_T;    /* AXI桥锁死状态寄存器1 */

typedef union
{
    struct
    {
        unsigned int    apb_pslv_active0           : 9; /* [8..0] apb slave口active信号状态0：bit[0]：1'b1（保留）bit[1]：wdogbit[2]：timer0～7bit[3]：1'b1(保留）bit[4]：1'b1(保留）bit[5]：1'b1(保留）bit[6]：1'b1(保留）bit[7]：uart0bit[8]：edmac */
        unsigned int    reserved                   : 23; /* [31..9] 保留。读返回常1 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT15_T;    /* slave active状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    dw_axi_bbphy_dlock_mst     : 2; /* [1..0] 总线死锁发生时，记录操作未完成的最小master号。0 - axi_glb1 - BBE16 DSP2 - BBP_MST13 - BBP_MST2 */
        unsigned int    dw_axi_bbphy_dlock_slv     : 3; /* [4..2] dlock_mst master正在访问的Slave号。0 - axi_glb1 - BBE16 DSP2 - BBP3 - 保留 */
        unsigned int    dw_axi_bbphy_dlock_wr      : 1; /* [5..5] 死锁传输读写指示。0：读；1：写。如果总线死锁发生时，读写操作都未完成，该信号指示写操作。 */
        unsigned int    reserved_1                 : 2; /* [7..6] 保留。 */
        unsigned int    dw_axi_bbphy_dlock_id      : 12; /* [19..8] 总线死锁时，记录的ID号。 */
        unsigned int    reserved_0                 : 12; /* [31..20] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT22_T;    /* AXI桥锁死状态寄存器2(BBPHY) */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 1; /* [0..0] 保留。 */
        unsigned int    ipf_idle                   : 1; /* [1..1] ipf是否空闲状态 */
        unsigned int    cicom0_clk_state           : 1; /* [2..2] cicom0是否空闲状态 */
        unsigned int    cicom1_clk_state           : 1; /* [3..3] cicom1是否空闲状态 */
        unsigned int    reserved_0                 : 28; /* [31..4] hao */
    } bits;
    unsigned int    u32;
}HI_SC_STAT26_T;    /* master状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    bbphy_slv_active           : 4; /* [3..0] dw_axi_bbphy桥slave口active信号状态：bit[0]：dw_axi_glbbit[1]：bbe16 dsp corebit[2]：bbpbit[3]：reserved */
        unsigned int    reserved_1                 : 4; /* [7..4] 保留。 */
        unsigned int    glb_slv_active             : 9; /* [16..8] dw_axi_glb桥slave口active信号状态：bit[8]：mdm acpbit[9]：reserved（常量1）bit[10]：保留（常量1）bit[11]：axi_membit[12]：x2h_peribit[13]：dw_axi_dspbit[14]：x2p（常量1）bit[15]：保留（常量1）bit[16]：axi_monitor */
        unsigned int    reserved_0                 : 15; /* [31..17] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT27_T;    /* slave active状态寄存器0 */

typedef union
{
    struct
    {
        unsigned int    x2h_peri_slv_active        : 32; /* [31..0] bit[0]：uiccbit[5:1]：保留，固定为1bit[6]：ipcmbit[7]：保留，固定为1bit[8]：hs_uartbit[10:9]：reservedbit[11]：cipherbit[12]：ipfbit[18:13]：reservedbit[19]：cicom0bit[20]：cicom1bit[24:21]：reservedbit[25]：bbe_vicbit[28:26]：reservedbit[29]：upacc                                          bit[30]：reservedbit[31]：reserved */
    } bits;
    unsigned int    u32;
}HI_SC_STAT29_T;    /* slave active状态寄存器2 */

typedef union
{
    struct
    {
        unsigned int    lte2soc_tbd                : 16; /* [15..0] sc_ctrl57[15:0] */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT32_T;    /* BBP保留状态寄存器。 */

typedef union
{
    struct
    {
        unsigned int    ap2mdm_key0                : 32; /* [31..0] Key0 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT35_T;    /* 安全Key寄存器0 */

typedef union
{
    struct
    {
        unsigned int    ap2mdm_key0                : 32; /* [31..0] Key1 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT36_T;    /* 安全Key寄存器1 */

typedef union
{
    struct
    {
        unsigned int    ap2mdm_key2                : 32; /* [31..0] Key2 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT37_T;    /* 安全Key寄存器2 */

typedef union
{
    struct
    {
        unsigned int    ap2mdm_key3                : 32; /* [31..0] Key3 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT38_T;    /* 安全Key寄存器3 */

typedef union
{
    struct
    {
        unsigned int    bbphy_mst_err              : 8; /* [7..0] dw_axi_bbphy桥访问错误寄存器。Bit[0]：m1口（dw_axi_glb）写访问错误。Bit[1]：m1口（dw_axi_glb）读访问错误。Bit[2]：m2口（dsp0）写访问错误。Bit[3]：m2口（dsp0）读访问错误。Bit[4]：m3口（bbp）写访问错误。Bit[5]：m3口（bbp）读访问错误。Bit[6]：m4口（reserved）写访问错误。Bit[7]：m4口（reserved）读访问错误。 */
        unsigned int    reserved                   : 12; /* [19..8] 保留。 */
        unsigned int    glb_mst_err                : 12; /* [31..20] dw_axi_glb桥访问错误寄存器。Bit[0]：m1口（dw_axi_bbphy）写访问错误Bit[1]：m1口（dw_axi_bbphy）读访问错误Bit[2]：m2口（dw_axi_mst）写访问错误Bit[3]：m2口（dw_axi_mst）读访问错误Bit[4]：m3口（app a9 m0口）写访问错误Bit[5]：m3口（app a9 m0口）读访问错误Bit[6]：m4口（mdm a9 m0口）写访问错误Bit[7]：m4口（mdm a9 m0口）读访问错误Bit[8]：m5口（reserved）写访问错误Bit[9]：m5口（reserved）读访问错误Bit[10]：m6口（hifi）写访问错误Bit[11]：m6口（hifi）读访问错误 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT41_T;    /* master访问错误状态寄存器0。 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 1; /* [0..0] 保留 */
        unsigned int    socapb_psel_err            : 1; /* [1..1] 当master访问不工作(关闭时钟，复位，掉电)的APB外设时，错误状态指示。该Bit只有在对应外设的slv_acitve_mask（SC_CTRL103）位配置为非屏蔽时才会产生。 */
        unsigned int    ahb_peri_mst_err           : 1; /* [2..2] 当master访问不工作(关闭时钟，复位，掉电)的APB外设时，错误状态指示。该Bit只有在对应外设的slv_acitve_mask（SC_CTRL105）位配置为非屏蔽时才会产生。 */
        unsigned int    reserved_0                 : 29; /* [31..3] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT42_T;    /* master访问错误状态寄存器1。 */

typedef union
{
    struct
    {
        unsigned int    x2h_peri_addr_err          : 32; /* [31..0] ahb外设访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT43_T;    /* ahb外设访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    reserved                   : 12; /* [11..0] 保留。 */
        unsigned int    addr_err                   : 20; /* [31..12] soc外设掉电域apb外设访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT44_T;    /* apb外设访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m1口（dw_axi_bbphy）写访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT46_T;    /* glb桥m1口（dw_axi_bbphy）写访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m1口（dw_axi_bbphy）读访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT47_T;    /* glb桥m1口（dw_axi_bbphy）读访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m2口（dw_axi_mst）写访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT48_T;    /* glb桥m2口（dw_axi_mst）写访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m2口（dw_axi_mst）读访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT49_T;    /* glb桥m2口（dw_axi_mst）读访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m3口（app a9 m0口）写访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT50_T;    /* glb桥m3口（app a9 m0口）写访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m3口（app a9 m0口）读访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT51_T;    /* glb桥m3口（app a9 m0口）读访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m4口（mdm a9 m0口）写访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT52_T;    /* glb桥m4口（mdm a9 m0口）写访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m4口（mdm a9 m0口）读访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT53_T;    /* glb桥m4口（mdm a9 m0口）读访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m5口（reserved）写访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT54_T;    /* glb桥m5口（reserved）写访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m5口（reserved）读访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT55_T;    /* glb桥m5口（reserved）读访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m6口（hifi）写访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT56_T;    /* glb桥m6口（hifi）写访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] glb桥m6口（hifi）读访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT57_T;    /* glb桥m6口（hifi）读访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] bbphy桥m1口（dw_axi_glb）写访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT62_T;    /* bbphy桥m1口（dw_axi_glb）写访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] bbphy桥m1口（dw_axi_glb）读访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT63_T;    /* bbphy桥m1口（dw_axi_glb）读访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] bbphy桥m2口（dsp0）写访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT64_T;    /* bbphy桥m2口（dsp0）写访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] bbphy桥m2口（dsp0）读访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT65_T;    /* bbphy桥m2口（dsp0）读访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] bbphy桥m3口（bbp）写访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT66_T;    /* bbphy桥m3口（bbp）写访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] bbphy桥m3口（bbp）读访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT67_T;    /* bbphy桥m3口（bbp）读访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] bbphy桥m4口（bbp）写访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT68_T;    /* bbphy桥m4口（bbp）写访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    addr_err                   : 32; /* [31..0] bbphy桥m4口（bbp）读访问错误地址寄存器。 */
    } bits;
    unsigned int    u32;
}HI_SC_STAT69_T;    /* bbphy桥m4口（bbp）读访问错误地址寄存器。 */

typedef union
{
    struct
    {
        unsigned int    reserved_1                 : 20; /* [19..0] 保留。 */
        unsigned int    sc_tcxo0_en_ctrl           : 1; /* [20..20] TCXO0 BUFFER使能控制。1 - 使能。0 - 不使能 */
        unsigned int    sc_tcxo1_en_ctrl           : 1; /* [21..21] TCXO1 BUFFER使能控制。1 - 使能。0 - 不使能 */
        unsigned int    reserved_0                 : 9; /* [30..22] 保留 */
        unsigned int    sc_tcxo_en_ctrl            : 1; /* [31..31] Modem子系统SOC部分TCXO时钟选择控制。0 - 使用来自AP的TCXO使能控制（tcxo？_clk_en）1 - 选择Modem系统的系统控制寄存器。参见bit20、21说明。 */
    } bits;
    unsigned int    u32;
}HI_PWR_CTRL2_T;    /* TCXO控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_3                 : 1; /* [0..0] 保留 */
        unsigned int    ccpu_iso_ctrl_en           : 1; /* [1..1] ModemA9电源域 ISO使能控制。1 - 隔离使能；0 - 无影响。 */
        unsigned int    reserved_2                 : 2; /* [3..2] 保留 */
        unsigned int    bbe16_iso_ctrl_en          : 1; /* [4..4] BBEDSP电源域 ISO使能控制。1 - 隔离使能；0 - 无影响。 */
        unsigned int    reserved_1                 : 1; /* [5..5] 保留 */
        unsigned int    ltebbp_iso_ctrl_en         : 1; /* [6..6] LTEBBP电源域 ISO使能控制。1 - 隔离使能；0 - 无影响。 */
        unsigned int    twbbp_iso_ctrl_en          : 1; /* [7..7] TWBBP电源域 ISO使能控制。1 - 隔离使能；0 - 无影响。 */
        unsigned int    wbbp_iso_ctrl_en           : 1; /* [8..8] WBBP电源域 ISO使能控制。1 - 隔离使能；0 - 无影响。 */
        unsigned int    g1bbp_iso_ctrl_en          : 1; /* [9..9] G1BBP电源域 ISO使能控制。1 - 隔离使能；0 - 无影响。 */
        unsigned int    g2bbp_iso_ctrl_en          : 1; /* [10..10] G2BBP电源域 ISO使能控制。1 - 隔离使能；0 - 无影响。 */
        unsigned int    irmbbp_iso_ctrl_en         : 1; /* [11..11] IRMBBP电源域 ISO使能控制。1 - 隔离使能；0 - 无影响。 */
        unsigned int    reserved_0                 : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_PWR_CTRL4_T;    /* ISOCELL隔离使能控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_3                 : 1; /* [0..0] 保留 */
        unsigned int    ccpu_iso_ctrl_dis          : 1; /* [1..1] ModemA9电源域 ISO使禁止控制。1 - 隔离禁止。0 - 无影响。 */
        unsigned int    reserved_2                 : 2; /* [3..2] 保留 */
        unsigned int    bbe16_iso_ctrl_dis         : 1; /* [4..4] BBEDSP电源域 ISO禁止控制。1 - 隔离禁止。0 - 无影响。 */
        unsigned int    reserved_1                 : 1; /* [5..5] 保留 */
        unsigned int    ltebbp_iso_ctrl_dis        : 1; /* [6..6] LTEBBP电源域 ISO禁止控制。1 - 隔离禁止。0 - 无影响。 */
        unsigned int    twbbp_iso_ctrl_dis         : 1; /* [7..7] TWBBP电源域 ISO禁止控制。1 - 隔离禁止。0 - 无影响。 */
        unsigned int    wbbp_iso_ctrl_dis          : 1; /* [8..8] WBBP电源域 ISO禁止控制。1 - 隔离禁止。0 - 无影响。 */
        unsigned int    g1bbp_iso_ctrl_dis         : 1; /* [9..9] G1BBP电源域 ISO禁止控制。1 - 隔离禁止。0 - 无影响。 */
        unsigned int    g2bbp_iso_ctrl_dis         : 1; /* [10..10] G2BBP电源域 ISO禁止控制。1 - 禁止。0 - 无影响。 */
        unsigned int    irmbbp_iso_ctrl_dis        : 1; /* [11..11] IRMBBP电源域 ISO禁止控制。1 - 隔离禁止。0 - 无影响。 */
        unsigned int    reserved_0                 : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_PWR_CTRL5_T;    /* ISOCELL隔离关闭控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_3                 : 1; /* [0..0] 保留 */
        unsigned int    ccpu_mtcmos_ctrl_en        : 1; /* [1..1] ModemA9电源域上下电使能控制。1 - 上电使能；0 - 无影响。 */
        unsigned int    reserved_2                 : 2; /* [3..2] 保留 */
        unsigned int    bbe16_mtcmos_ctrl_en       : 1; /* [4..4] BBEDSP电源域上下电使能控制。1 - 上电使能；0 - 无影响。 */
        unsigned int    reserved_1                 : 1; /* [5..5] 保留 */
        unsigned int    ltebbp_mtcmos_ctrl_en      : 1; /* [6..6] LTEBBP电源域上下电使能控制。1 - 上电使能；0 - 无影响。 */
        unsigned int    twbbp_mtcmos_ctrl_en       : 1; /* [7..7] TWBBP电源域上下电使能控制。1 - 上电使能；0 - 无影响。 */
        unsigned int    wbbp_mtcmos_ctrl_en        : 1; /* [8..8] WBBP电源域 上下电使能控制。1 - 上电使能；0 - 无影响。 */
        unsigned int    g1bbp_mtcmos_ctrl_en       : 1; /* [9..9] G1BBP电源域 上下电使能控制。1 - 上电使能；0 - 无影响。 */
        unsigned int    g2bbp_mtcmos_ctrl_en       : 1; /* [10..10] G2BBP电源域上下电使能控制。1 - 上电使能；0 - 无影响。 */
        unsigned int    irmbbp_mtcmos_ctrl_en      : 1; /* [11..11] IRMBBP电源域上下电使能控制。1 - 上电使能；0 - 无影响。 */
        unsigned int    reserved_0                 : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_PWR_CTRL6_T;    /* MTCMOS上电使能控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_3                 : 1; /* [0..0] 保留 */
        unsigned int    ccpu_mtcmos_ctrl_dis       : 1; /* [1..1] ModemA9电源域下电使能控制。1 - 下电使能；0 - 无影响。 */
        unsigned int    reserved_2                 : 2; /* [3..2] 保留 */
        unsigned int    bbe16_mtcmos_ctrl_dis      : 1; /* [4..4] BBEDSP电源域下电使能控制。1 - 下电使能；0 - 无影响。 */
        unsigned int    reserved_1                 : 1; /* [5..5] 保留 */
        unsigned int    ltebbp_mtcmos_ctrl_dis     : 1; /* [6..6] LTEBBP电源域下电使能控制。1 - 下电使能；0 - 无影响。 */
        unsigned int    twbbp_mtcmos_ctrl_dis      : 1; /* [7..7] TWBBP电源域下电使能控制。1 - 下电使能；0 - 无影响。 */
        unsigned int    wbbp_mtcmos_ctrl_dis       : 1; /* [8..8] WBBP电源域 下电使能控制。1 - 下电使能；0 - 无影响。 */
        unsigned int    g1bbp_mtcmos_ctrl_dis      : 1; /* [9..9] G1BBP电源域 下电使能控制。1 - 下电使能；0 - 无影响。 */
        unsigned int    g2bbp_mtcmos_ctrl_dis      : 1; /* [10..10] G2BBP电源域下电使能控制。1 - 下电使能；0 - 无影响。 */
        unsigned int    irmbbp_mtcmos_ctrl_dis     : 1; /* [11..11] IRMBBP电源域下电使能控制。1 - 下电使能；0 - 无影响。 */
        unsigned int    reserved_0                 : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_PWR_CTRL7_T;    /* MTCMOS下电使能控制寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_4                 : 1; /* [0..0] 保留 */
        unsigned int    ccpu_mtcmos_rdy_stat       : 1; /* [1..1] ModemA9电源域上下电状态指示。1 - 上电状态；0 - 下电状态。 */
        unsigned int    reserved_3                 : 2; /* [3..2] 保留 */
        unsigned int    bbe16_mtcmos_rdy_stat      : 1; /* [4..4] BBEDSP电源域上下电状态指示。1 - 上电状态；0 - 下电状态。 */
        unsigned int    reserved_2                 : 1; /* [5..5] 保留 */
        unsigned int    ltebbp_mtcmos_rdy_stat     : 1; /* [6..6] LTEBBP电源域上下电状态指示。1 - 上电状态；0 - 下电状态。 */
        unsigned int    twbbp_mtcmos_rdy_stat      : 1; /* [7..7] TWBBP电源域上下电状态指示。1 - 上电状态；0 - 下电状态。 */
        unsigned int    wbbp_mtcmos_rdy_stat       : 1; /* [8..8] WBBP电源域上下电状态指示。1 - 上电状态；0 - 下电状态。 */
        unsigned int    g1bbp_mtcmos_rdy_stat      : 1; /* [9..9] G1BBP电源域上下电状态指示。1 - 上电状态；0 - 下电状态。 */
        unsigned int    g2bbp_mtcmos_rdy_stat      : 1; /* [10..10] G2BBP电源域上下电状态指示。1 - 上电状态；0 - 下电状态。 */
        unsigned int    irmbbp_mtcmos_rdy_stat     : 1; /* [11..11] IRMBBP电源域上下电状态指示。1 - 上电状态；0 - 下电状态。 */
        unsigned int    reserved_1                 : 2; /* [13..12] 保留。 */
        unsigned int    ltebbp0_mtcmos_rdy_stat    : 1; /* [14..14] LTEBBP0电源域上下电状态指示。1 - 上电状态；0 - 下电状态。LTEBBP0与LTEBBP为同一个电源域，采用同一个上下电和隔离控制。 */
        unsigned int    reserved_0                 : 17; /* [31..15] 保留 */
    } bits;
    unsigned int    u32;
}HI_PWR_STAT1_T;    /* MTCMOS_RDY状态指示寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_3                 : 1; /* [0..0] 保留 */
        unsigned int    ccpu_iso_ctrl_stat         : 1; /* [1..1] ModemA9电源域 ISO控制状态指示。1 - 隔离使能；0 - 隔离禁止。 */
        unsigned int    reserved_2                 : 2; /* [3..2] 保留 */
        unsigned int    bbe16_iso_ctrl_stat        : 1; /* [4..4] BBEDSP电源域 ISO控制状态指示。1 - 隔离使能；0 - 隔离禁止。 */
        unsigned int    reserved_1                 : 1; /* [5..5] 保留 */
        unsigned int    ltebbp_iso_ctrl_stat       : 1; /* [6..6] LTEBBP电源域 ISO控制状态指示。1 - 隔离使能；0 - 隔离禁止。 */
        unsigned int    twbbp_iso_ctrl_stat        : 1; /* [7..7] TWBBP电源域 ISO控制状态指示。1 - 隔离使能；0 - 隔离禁止。 */
        unsigned int    wbbp_iso_ctrl_stat         : 1; /* [8..8] WBBP电源域 ISO控制状态指示。1 - 隔离使能；0 - 隔离禁止。 */
        unsigned int    g1bbp_iso_ctrl_stat        : 1; /* [9..9] G1BBP电源域 ISO控制状态指示。1 - 隔离使能；0 - 隔离禁止。 */
        unsigned int    g2bbp_iso_ctrl_stat        : 1; /* [10..10] G2BBP电源域 ISO控制状态指示。1 - 隔离使能；0 - 隔离禁止。 */
        unsigned int    irmbbp_iso_ctrl_stat       : 1; /* [11..11] IRMBBP电源域 ISO控制状态指示。1 - 隔离使能；0 - 隔离禁止。 */
        unsigned int    reserved_0                 : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_PWR_STAT3_T;    /* ISOCELL隔离控制状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    reserved_3                 : 1; /* [0..0] 保留 */
        unsigned int    ccpu_mtcmos_ctrl_stat      : 1; /* [1..1] ModemA9电源域上下电控制状态指示。1 - 上电使能；0 - 下电使能。 */
        unsigned int    reserved_2                 : 2; /* [3..2] 保留 */
        unsigned int    bbe16_mtcmos_ctrl_stat     : 1; /* [4..4] BBEDSP电源域 上下电控制状态指示。1 - 上电使能；0 - 下电使能。 */
        unsigned int    reserved_1                 : 1; /* [5..5] 保留 */
        unsigned int    ltebbp_mtcmos_ctrl_stat    : 1; /* [6..6] LTEBBP电源域上下电控制状态指示。1 - 上电使能；0 - 下电使能。 */
        unsigned int    twbbp_mtcmos_ctrl_stat     : 1; /* [7..7] TWBBP电源域上下电控制状态指示。1 - 上电使能；0 - 下电使能。 */
        unsigned int    wbbp_mtcmos_ctrl_stat      : 1; /* [8..8] WBBP电源域上下电控制状态指示。1 - 上电使能；0 - 下电使能。 */
        unsigned int    g1bbp_mtcmos_ctrl_stat     : 1; /* [9..9] G1BBP电源域上下电控制状态指示。1 - 上电使能；0 - 下电使能。 */
        unsigned int    g2bbp_mtcmos_ctrl_stat     : 1; /* [10..10] G2BBP电源域上下电控制状态指示。1 - 上电使能；0 - 下电使能。 */
        unsigned int    irmbbp_mtcmos_ctrl_stat    : 1; /* [11..11] IRMBBP电源域上下电控制状态指示。1 - 上电使能；0 - 下电使能。 */
        unsigned int    reserved_0                 : 20; /* [31..12] 保留 */
    } bits;
    unsigned int    u32;
}HI_PWR_STAT4_T;    /* MTCMOS上下电控制状态寄存器 */

typedef union
{
    struct
    {
        unsigned int    ipf_sec_w_ctrl             : 1; /* [0..0] AP侧访问IPF安全属性控制。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    ipf_sec_r_ctrl             : 1; /* [1..1] AP侧访问IPF安全属性控制。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    region1_sec_w_ctrl         : 1; /* [2..2] AP侧访问Modem子系统Region1读安全属性控制。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    region1_sec_r_ctrl         : 1; /* [3..3] AP侧访问Modem子系统Region1写安全属性控制（可访问空间见安全空间寄存器2.3）。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    ipcm_sec_w_ctrl            : 1; /* [4..4] AP侧访问IPCM安全属性控制。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    ipcm_sec_r_ctrl            : 1; /* [5..5] AP侧访问IPCM安全属性控制。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    aximon_sec_w_ctrl          : 1; /* [6..6] AP侧访问AXIMON安全属性控制。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    aximon_sec_r_ctrl          : 1; /* [7..7] AP侧访问AXIMON安全属性控制。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    others_sec_w_ctrl          : 1; /* [8..8] AP侧访问其他地址安全属性控制。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    others_sec_r_ctrl          : 1; /* [9..9] AP侧访问其他地址安全属性控制。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    region2_sec_r_ctrl         : 2; /* [11..10] 保留 */
        unsigned int    uicc_sec_w_ctrl            : 1; /* [12..12] AP侧访问UICC读安全属性控制。0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    uicc_sec_r_ctrl            : 1; /* [13..13] AP侧访问UICC写安全属性控制0 - 非安全可访问；1 - 只能安全访问。 */
        unsigned int    reserved                   : 18; /* [31..14] 保留 */
    } bits;
    unsigned int    u32;
}HI_SEC_CTRL0_T;    /* 安全控制寄存器0(slave接口） */

typedef union
{
    struct
    {
        unsigned int    noddr_bypass               : 1; /* [0..0] 非DDR空间直通配置。1 - bypass(default)0 - 非bypass */
        unsigned int    reserved                   : 15; /* [15..1] 保留 */
        unsigned int    sci0_sec_w_ctrl            : 1; /* [16..16] modem子系统访问SCI0安全属性控制（写操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    sci0_sec_r_ctrl            : 1; /* [17..17] modem子系统访问SCI0安全属性控制（读操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    sci1_sec_w_ctrl            : 1; /* [18..18] modem子系统访问SCI1安全属性控制（写操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    sci1_sec_r_ctrl            : 1; /* [19..19] modem子系统访问SCI1安全属性控制（读操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    ipcm_s_sec_w_ctrl          : 1; /* [20..20] modem子系统访问IPCM_S安全属性控制（写操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    ipcm_s_sec_r_ctrl          : 1; /* [21..21] modem子系统访问IPCM_S安全属性控制（读操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    ipcm_ns_sec_w_ctrl         : 1; /* [22..22] modem子系统访问IPCM_NS安全属性控制（写操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    ipcm_ns_sec_r_ctrl         : 1; /* [23..23] modem子系统访问IPCM_NS安全属性控制（读操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    secram_sec_w_ctrl          : 1; /* [24..24] modem子系统访问SECRAM安全属性控制（写操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    secram_sec_r_ctrl          : 1; /* [25..25] modem子系统访问SECRAM安全属性控制（读操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    socp_sec_w_ctrl            : 1; /* [26..26] modem子系统访问SSOCP安全属性控制（写操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    socp_sec_r_ctrl            : 1; /* [27..27] modem子系统访问SOCP安全属性控制（读操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    others_sec_w_ctrl          : 1; /* [28..28] modem子系统访问SSOCP安全属性控制（写操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    others_sec_r_ctrl          : 1; /* [29..29] modem子系统访问其他地址空间安全属性控制（读操作）。0 - 发起非安全操作；1 - 发起安全操作 */
        unsigned int    ap_sec_ctrl                : 2; /* [31..30] L2C master接口安全控制。Bit[31] - bypass控制0 - a*prot透传(default)1 - a*prot由bit[30]控制。Bit[30] - 非bypass模式下，a*prot[1]控制。1 - 发起安全操作；0 - 发起非安全操作（default） */
    } bits;
    unsigned int    u32;
}HI_SEC_CTRL1_T;    /* 安全控制寄存器1(GLB_MASTER接口） */

typedef union
{
    struct
    {
        unsigned int    region1_filter_st_addr     : 32; /* [31..0] AP侧访问Modem子系统region1的起始地址 */
    } bits;
    unsigned int    u32;
}HI_SEC_CTRL2_T;    /* 安全控制寄存器2 */

typedef union
{
    struct
    {
        unsigned int    region1_filter_end_addr    : 32; /* [31..0] AP侧访问Modem子系统region1的结束地址 */
    } bits;
    unsigned int    u32;
}HI_SEC_CTRL3_T;    /* 安全控制寄存器3 */

typedef union
{
    struct
    {
        unsigned int    secram_filter_st_addr      : 32; /* [31..0] SECRA访问的起始地址。Modem访问该空间的安全特性可独立配置，参见安全控制寄存器1 */
    } bits;
    unsigned int    u32;
}HI_SEC_CTRL4_T;    /* 安全控制寄存器4 */

typedef union
{
    struct
    {
        unsigned int    secram_filter_end_addr     : 32; /* [31..0] SECRAM访问的结束地址 */
    } bits;
    unsigned int    u32;
}HI_SEC_CTRL5_T;    /* 安全控制寄存器5 */


/********************************************************************************/
/*    syssc 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
#ifndef __KERNEL__
HI_SET_GET(hi_crg_clken3_reserved_1,reserved_1,HI_CRG_CLKEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN3_OFFSET)
HI_SET_GET(hi_crg_clken3_bbe_refclk_en,bbe_refclk_en,HI_CRG_CLKEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN3_OFFSET)
HI_SET_GET(hi_crg_clken3_a9_refclk_en,a9_refclk_en,HI_CRG_CLKEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN3_OFFSET)
HI_SET_GET(hi_crg_clken3_hpm_clk_en,hpm_clk_en,HI_CRG_CLKEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN3_OFFSET)
HI_SET_GET(hi_crg_clken3_reserved_0,reserved_0,HI_CRG_CLKEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN3_OFFSET)
HI_SET_GET(hi_crg_clkdis3_reserved_1,reserved_1,HI_CRG_CLKDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS3_OFFSET)
HI_SET_GET(hi_crg_clkdis3_bbe_refclk_dis,bbe_refclk_dis,HI_CRG_CLKDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS3_OFFSET)
HI_SET_GET(hi_crg_clkdis3_a9_refclk_dis,a9_refclk_dis,HI_CRG_CLKDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS3_OFFSET)
HI_SET_GET(hi_crg_clkdis3_hpm_clk_dis,hpm_clk_dis,HI_CRG_CLKDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS3_OFFSET)
HI_SET_GET(hi_crg_clkdis3_reserved_0,reserved_0,HI_CRG_CLKDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS3_OFFSET)
HI_SET_GET(hi_crg_clkstat3_reserved_1,reserved_1,HI_CRG_CLKSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT3_OFFSET)
HI_SET_GET(hi_crg_clkstat3_bbe_refclk_status,bbe_refclk_status,HI_CRG_CLKSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT3_OFFSET)
HI_SET_GET(hi_crg_clkstat3_a9_refclk_status,a9_refclk_status,HI_CRG_CLKSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT3_OFFSET)
HI_SET_GET(hi_crg_clkstat3_hpm_clk_status,hpm_clk_status,HI_CRG_CLKSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT3_OFFSET)
HI_SET_GET(hi_crg_clkstat3_reserved_0,reserved_0,HI_CRG_CLKSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT3_OFFSET)
HI_SET_GET(hi_crg_clken4_bbe_pd_clk_en,bbe_pd_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_bbe_core_clk_en,bbe_core_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_reserved_2,reserved_2,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_mcpu_pd_clk_en,mcpu_pd_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_l2c_clk_en,l2c_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_reserved_1,reserved_1,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_mcpu_mbist_clk_en,mcpu_mbist_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_reserved_0,reserved_0,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_cicom0_clk_en,cicom0_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_cicom1_clk_en,cicom1_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_ipf_clk_en,ipf_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_upacc_clk_en,upacc_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_sci0_clk_en,sci0_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_sci1_clk_en,sci1_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_uicc_clk_en,uicc_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_uart_clk_en,uart_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_bbe_vic_clk_en,bbe_vic_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_cipher_clk_en,cipher_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_edmac_clk_en,edmac_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_ipcm_clk_en,ipcm_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_hsuart_clk_en,hsuart_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_aximem_clk_en,aximem_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_aximon_clk_en,aximon_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_timer_clk_en,timer_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clken4_wdog_clk_en,wdog_clk_en,HI_CRG_CLKEN4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_dsp0_pd_clk_dis,dsp0_pd_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_dsp0_core_clk_dis,dsp0_core_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_dsp0_dbg_clk_dis,dsp0_dbg_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_mcpu_pd_clk_dis,mcpu_pd_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_l2c_clk_dis,l2c_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_mcpu_dbg_clk_dis,mcpu_dbg_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_mcpu_mbist_clk_dis,mcpu_mbist_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_reserved,reserved,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_cicom0_clk_dis,cicom0_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_cicom1_clk_dis,cicom1_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_ipf_clk_dis,ipf_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_upacc_clk_dis,upacc_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_sci0_clk_dis,sci0_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_sci1_clk_dis,sci1_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_uicc_clk_dis,uicc_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_uart_clk_dis,uart_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_bbe_vic_clk_dis,bbe_vic_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_cipher_clk_dis,cipher_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_edmac_clk_dis,edmac_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_ipcm_clk_dis,ipcm_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_hsuart_clk_dis,hsuart_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_aximem_clk_dis,aximem_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_aximon_clk_dis,aximon_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_timer_clk_dis,timer_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkdis4_wdog_clk_dis,wdog_clk_dis,HI_CRG_CLKDIS4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_bbe_pd_clk_status,bbe_pd_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_bbe_core_clk_status,bbe_core_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_bbe_dbg_clk_status,bbe_dbg_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_mcpu_pd_clk_status,mcpu_pd_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_l2c_clk_status,l2c_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_mcpu_dbg_clk_status,mcpu_dbg_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_mcpu_mbist_clk_status,mcpu_mbist_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_reserved,reserved,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_cicom0_clk_status,cicom0_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_cicom1_clk_status,cicom1_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_ipf_clk_status,ipf_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_upacc_clk_status,upacc_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_sci0_clk_status,sci0_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_sci1_clk_status,sci1_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_uicc_clk_status,uicc_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_uart_clk_status,uart_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_bbe_vic_clk_status,bbe_vic_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_cipher_clk_status,cipher_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_edmac_clk_status,edmac_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_ipcm_clk_status,ipcm_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_hsuart_clk_status,hsuart_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_aximem_clk_status,aximem_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_aximon_clk_status,aximon_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_timer_clk_status,timer_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clkstat4_wdog_clk_status,wdog_clk_status,HI_CRG_CLKSTAT4_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT4_OFFSET)
HI_SET_GET(hi_crg_clken5_bbp_ao_clk_en,bbp_ao_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_bbpcommon_2a_clk_en,bbpcommon_2a_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_ltebbp_pd_clk_en,ltebbp_pd_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_g1bbp_pd_clk_en,g1bbp_pd_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_g2bbp_pd_clk_en,g2bbp_pd_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_twbbp_pd_clk_en,twbbp_pd_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_wbbp_pd_clk_en,wbbp_pd_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_irm_pd_clk_en,irm_pd_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_lbbp_axi_clk_en,lbbp_axi_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_lbbp_pdt_clk_en,lbbp_pdt_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_lbbp_pdf_clk_en,lbbp_pdf_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_lbbp_tdl_clk_en,lbbp_tdl_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_lbbp_vdl_clk_en,lbbp_vdl_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_lbbp_tds_clk_en,lbbp_tds_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_reserved_1,reserved_1,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_irm_turbo_clk_en,irm_turbo_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_irm_bbc_245m_clk_en,irm_bbc_245m_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_irm_wbbp_122m_clk_en,irm_wbbp_122m_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_twbbp_wbbp_122m_clk_en,twbbp_wbbp_122m_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_twbbp_wbbp_61m_clk_en,twbbp_wbbp_61m_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_twbbp_tbbp_122m_clk_en,twbbp_tbbp_122m_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_twbbp_tbbp_245m_clk_en,twbbp_tbbp_245m_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_wbbp_61m_clk_en,wbbp_61m_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_wbbp_122m_clk_en,wbbp_122m_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_wbbp_245m_clk_en,wbbp_245m_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_wbbp_axi_clk_en,wbbp_axi_clk_en,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clken5_reserved_0,reserved_0,HI_CRG_CLKEN5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKEN5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_bbp_ao_clk_dis,bbp_ao_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_bbpcommon_2a_clk_dis,bbpcommon_2a_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_lbbp_pd_clk_dis,lbbp_pd_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_g1bbp_pd_clk_dis,g1bbp_pd_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_g2bbp_pd_clk_dis,g2bbp_pd_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_twbbp_pd_clk_dis,twbbp_pd_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_wbbp_pd_clk_dis,wbbp_pd_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_irmbbp_pd_clk_dis,irmbbp_pd_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_lbbp_axi_clk_dis,lbbp_axi_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_lbbp_pdt_clk_dis,lbbp_pdt_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_lbbp_pdf_clk_dis,lbbp_pdf_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_lbbp_tdl_clk_dis,lbbp_tdl_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_lbbp_vdl_clk_dis,lbbp_vdl_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_lbbp_tds_clk_dis,lbbp_tds_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_reserved_1,reserved_1,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_irm_turbo_clk_dis,irm_turbo_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_irm_bbc_245m_clk_dis,irm_bbc_245m_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_irm_wbbp_clk_dis,irm_wbbp_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_twbbp_wbbp_122m_clk_dis,twbbp_wbbp_122m_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_twbbp_wbbp_61m_clk_dis,twbbp_wbbp_61m_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_twbbp_tbbp_122m_clk_dis,twbbp_tbbp_122m_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_twbbp_tbbp_245m_clk_dis,twbbp_tbbp_245m_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_wbbp_61m_clk_dis,wbbp_61m_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_wbbp_122m_clk_dis,wbbp_122m_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_wbbp_245m_clk_dis,wbbp_245m_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_wbbp_axi_clk_dis,wbbp_axi_clk_dis,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkdis5_reserved_0,reserved_0,HI_CRG_CLKDIS5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIS5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_bbp_ao_clk_stat,bbp_ao_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_bbpcom_clk_stat,bbpcom_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_lbbp_pd_clk_stat,lbbp_pd_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_g1bbp_pd_clk_stat,g1bbp_pd_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_g2bbp_pd_clk_stat,g2bbp_pd_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_twbbp_pd_clk_stat,twbbp_pd_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_wbbp_pd_clk_stat,wbbp_pd_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_irmbbp_pd_clk_stat,irmbbp_pd_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_lbbp_axi_clk_stat,lbbp_axi_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_lbbp_pdt_clk_stat,lbbp_pdt_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_lbbp_pdf_clk_stat,lbbp_pdf_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_lbbp_tdl_clk_stat,lbbp_tdl_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_lbbp_vdl_clk_stat,lbbp_vdl_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_lbbp_tds_clk_stat,lbbp_tds_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_reserved_1,reserved_1,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_irm_turbo_clk_stat,irm_turbo_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_irm_bbc_245m_clk_stat,irm_bbc_245m_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_irm_wbbp_clk_stat,irm_wbbp_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_twbbp_wbbp_122m_clk_stat,twbbp_wbbp_122m_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_twbbp_wbbp_61m_clk_stat,twbbp_wbbp_61m_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_twbbp_tbbp_122m_clk_stat,twbbp_tbbp_122m_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_twbbp_tbbp_245m_clk_stat,twbbp_tbbp_245m_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_wbbp_61m_clk_stat,wbbp_61m_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_wbbp_122m_clk_stat,wbbp_122m_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_wbbp_245m_clk_stat,wbbp_245m_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_wbbp_axi_clk_stat,wbbp_axi_clk_stat,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_clkstat5_reserved_0,reserved_0,HI_CRG_CLKSTAT5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKSTAT5_OFFSET)
HI_SET_GET(hi_crg_srsten1_bbe_pd_srst_en,bbe_pd_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_bbe_core_srst_en,bbe_core_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_bbe_dbg_srst_en,bbe_dbg_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_reserved_4,reserved_4,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_moda9_cpu_srst_en,moda9_cpu_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_moda9_dbg_srst_en,moda9_dbg_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_moda9_peri_srst_en,moda9_peri_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_moda9_scu_srst_en,moda9_scu_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_moda9_ptm_srst_en,moda9_ptm_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_moda9_wd_srst_en,moda9_wd_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_reserved_3,reserved_3,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_mdma9_pd_srst_en,mdma9_pd_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_reserved_2,reserved_2,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_uicc_srst_en,uicc_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_reserved_1,reserved_1,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_amon_soc_srst_en,amon_soc_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_reserved_0,reserved_0,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srsten1_hpm_srst_en,hpm_srst_en,HI_CRG_SRSTEN1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_bbe_pd_srst_dis,bbe_pd_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_bbe_core_srst_dis,bbe_core_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_bbe_dbg_srst_dis,bbe_dbg_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_reserved_4,reserved_4,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_moda9_core_srst_dis,moda9_core_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_moda9_dbg_srst_dis,moda9_dbg_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_moda9_peri_srst_dis,moda9_peri_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_moda9_scu_srst_dis,moda9_scu_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_moda9_ptm_srst_dis,moda9_ptm_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_moda9_wd_srst_dis,moda9_wd_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_reserved_3,reserved_3,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_mdma9_pd_srst_dis,mdma9_pd_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_reserved_2,reserved_2,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_uicc_srst_dis,uicc_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_reserved_1,reserved_1,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_amon_soc_srst_dis,amon_soc_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_reserved_0,reserved_0,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srstdis1_hpm_srst_dis,hpm_srst_dis,HI_CRG_SRSTDIS1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS1_OFFSET)
HI_SET_GET(hi_crg_srststat1_bbe_pd_srst_status,bbe_pd_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_bbe_core_srst_status,bbe_core_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_bbe_dbg_srst_status,bbe_dbg_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_reserved_4,reserved_4,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_moda9_core_srst_status,moda9_core_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_moda9_dbg_srst_status,moda9_dbg_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_moda9_peri_srst_status,moda9_peri_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_moda9_scu_srst_status,moda9_scu_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_moda9_ptm_srst_status,moda9_ptm_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_moda9_wd_srst_status,moda9_wd_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_reserved_3,reserved_3,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_mdma9_pd_srst_status,mdma9_pd_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_reserved_2,reserved_2,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_uicc_srst_status,uicc_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_reserved_1,reserved_1,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_amon_soc_srst_status,amon_soc_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_reserved_0,reserved_0,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srststat1_hpm_srst_status,hpm_srst_status,HI_CRG_SRSTSTAT1_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT1_OFFSET)
HI_SET_GET(hi_crg_srsten3_bbp_crg_srst_en,bbp_crg_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_bbpcommon_2a_srst_en,bbpcommon_2a_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_lbbp_pd_srst_en,lbbp_pd_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_g1bbp_pd_srst_en,g1bbp_pd_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_g2bbp_pd_srst_en,g2bbp_pd_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_twbbp_pd_srst_en,twbbp_pd_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_wbbp_pd_srst_en,wbbp_pd_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_irm_pd_srst_en,irm_pd_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_bbc_lbbp_pdt_srst_en,bbc_lbbp_pdt_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_bbc_lbbp_pdf_srst_en,bbc_lbbp_pdf_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_bbc_lbbp_tdl_srst_en,bbc_lbbp_tdl_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_bbc_tbbp_245m_srst_en,bbc_tbbp_245m_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_bbc_twbbp_122m_srst_en,bbc_twbbp_122m_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_bbc_g1bbp_104m_srst_en,bbc_g1bbp_104m_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_bbc_g2bbp_104m_srst_en,bbc_g2bbp_104m_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_reserved,reserved,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srsten3_abb_srst_en,abb_srst_en,HI_CRG_SRSTEN3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTEN3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_bbp_crg_srst_dis,bbp_crg_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_bbpcommon_2a_pd_srst_dis,bbpcommon_2a_pd_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_lbbp_pd_srst_dis,lbbp_pd_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_g1bbp_pd_srst_dis,g1bbp_pd_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_g2bbp_pd_srst_dis,g2bbp_pd_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_twbbp_pd_srst_dis,twbbp_pd_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_wbbp_pd_srst_dis,wbbp_pd_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_irm_pd_srst_dis,irm_pd_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_bbc_lbbp_pdt_srst_dis,bbc_lbbp_pdt_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_bbc_lbbp_pdf_srst_dis,bbc_lbbp_pdf_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_bbc_lbbp_tdl_srst_dis,bbc_lbbp_tdl_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_bbc_tbbp_245m_srst_dis,bbc_tbbp_245m_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_bbc_twbbp_122m_srst_dis,bbc_twbbp_122m_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_bbc_g1bbp_104m_srst_dis,bbc_g1bbp_104m_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_bbc_g2bbp_104m_srst_dis,bbc_g2bbp_104m_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_reserved,reserved,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srstdis3_abb_srst_dis,abb_srst_dis,HI_CRG_SRSTDIS3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTDIS3_OFFSET)
HI_SET_GET(hi_crg_srststat3_bbp_crg_srst_stat,bbp_crg_srst_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_bbpcommon_2a_srst_stat,bbpcommon_2a_srst_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_lbbp_pd_srst_stat,lbbp_pd_srst_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_g1bbp_pd_srst_stat,g1bbp_pd_srst_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_g2bbp_pd_srst_stat,g2bbp_pd_srst_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_twbbp_pd_srst_stat,twbbp_pd_srst_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_wbbp_pd_srst_stat,wbbp_pd_srst_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_irm_pd_srst_stat,irm_pd_srst_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_bbc_lbbp_pdt_srstctrl_stat,bbc_lbbp_pdt_srstctrl_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_bbc_lbbp_pdf_srstctrl_stat,bbc_lbbp_pdf_srstctrl_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_bbc_lbbp_tdl_srstctrl_stat,bbc_lbbp_tdl_srstctrl_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_bbc_tbbp_245m_srstctrl_stat,bbc_tbbp_245m_srstctrl_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_bbc_twbbp_122m_srstctrl_stat,bbc_twbbp_122m_srstctrl_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_bbc_g1bbp_104m_srstctrl_stat,bbc_g1bbp_104m_srstctrl_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_bbc_g2bbp_104m_srstctrl_stat,bbc_g2bbp_104m_srstctrl_stat,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_reserved,reserved,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_srststat3_abb_srst_status,abb_srst_status,HI_CRG_SRSTSTAT3_T,HI_SYSSC_BASE_ADDR, HI_CRG_SRSTSTAT3_OFFSET)
HI_SET_GET(hi_crg_clkdiv2_reserved_4,reserved_4,HI_CRG_CLKDIV2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV2_OFFSET)
HI_SET_GET(hi_crg_clkdiv2_mdma9_clk_div,mdma9_clk_div,HI_CRG_CLKDIV2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV2_OFFSET)
HI_SET_GET(hi_crg_clkdiv2_a92slow_freqmode,a92slow_freqmode,HI_CRG_CLKDIV2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV2_OFFSET)
HI_SET_GET(hi_crg_clkdiv2_reserved_3,reserved_3,HI_CRG_CLKDIV2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV2_OFFSET)
HI_SET_GET(hi_crg_clkdiv2_a92fast_freqmode,a92fast_freqmode,HI_CRG_CLKDIV2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV2_OFFSET)
HI_SET_GET(hi_crg_clkdiv2_reserved_2,reserved_2,HI_CRG_CLKDIV2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV2_OFFSET)
HI_SET_GET(hi_crg_clkdiv2_a9_clksw_req,a9_clksw_req,HI_CRG_CLKDIV2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV2_OFFSET)
HI_SET_GET(hi_crg_clkdiv2_reserved_1,reserved_1,HI_CRG_CLKDIV2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV2_OFFSET)
HI_SET_GET(hi_crg_clkdiv2_apb_freqmode,apb_freqmode,HI_CRG_CLKDIV2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV2_OFFSET)
HI_SET_GET(hi_crg_clkdiv2_reserved_0,reserved_0,HI_CRG_CLKDIV2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV2_OFFSET)
HI_SET_GET(hi_crg_clkdiv5_bbe_clk_div_num,bbe_clk_div_num,HI_CRG_CLKDIV5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV5_OFFSET)
HI_SET_GET(hi_crg_clkdiv5_reserved_2,reserved_2,HI_CRG_CLKDIV5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV5_OFFSET)
HI_SET_GET(hi_crg_clkdiv5_bbe_freqmode,bbe_freqmode,HI_CRG_CLKDIV5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV5_OFFSET)
HI_SET_GET(hi_crg_clkdiv5_reserved_1,reserved_1,HI_CRG_CLKDIV5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV5_OFFSET)
HI_SET_GET(hi_crg_clkdiv5_cipher_clk_div,cipher_clk_div,HI_CRG_CLKDIV5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV5_OFFSET)
HI_SET_GET(hi_crg_clkdiv5_reserved_0,reserved_0,HI_CRG_CLKDIV5_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLKDIV5_OFFSET)
HI_SET_GET(hi_crg_clk_sel2_timer_clk_sel,timer_clk_sel,HI_CRG_CLK_SEL2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL2_OFFSET)
HI_SET_GET(hi_crg_clk_sel2_gps_tcxo_clk_sel,gps_tcxo_clk_sel,HI_CRG_CLK_SEL2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL2_OFFSET)
HI_SET_GET(hi_crg_clk_sel2_hpm_refclk_sel,hpm_refclk_sel,HI_CRG_CLK_SEL2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL2_OFFSET)
HI_SET_GET(hi_crg_clk_sel2_reserved_1,reserved_1,HI_CRG_CLK_SEL2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL2_OFFSET)
HI_SET_GET(hi_crg_clk_sel2_sc_a9wdt_rst_en,sc_a9wdt_rst_en,HI_CRG_CLK_SEL2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL2_OFFSET)
HI_SET_GET(hi_crg_clk_sel2_reserved_0,reserved_0,HI_CRG_CLK_SEL2_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL2_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_abb_tcxo_clk_sel,abb_tcxo_clk_sel,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_bbppll_refclk_sel,bbppll_refclk_sel,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_bbp_tcxo_clk_sel,bbp_tcxo_clk_sel,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_ch0_tcxo_clk_sel,ch0_tcxo_clk_sel,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_ch1_tcxo_clk_sel,ch1_tcxo_clk_sel,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_g1bbp_104m_clk_sel,g1bbp_104m_clk_sel,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_g2bbp_104m_clk_sel,g2bbp_104m_clk_sel,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_tdl_clk_sel,tdl_clk_sel,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_lbbp_vdl_clk_sel,lbbp_vdl_clk_sel,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_lbbp_pdf_clk_sel,lbbp_pdf_clk_sel,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_clk_sel3_reserved,reserved,HI_CRG_CLK_SEL3_T,HI_SYSSC_BASE_ADDR, HI_CRG_CLK_SEL3_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg0_pll_en,pll_en,HI_CRG_A9PLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg0_pll_bp,pll_bp,HI_CRG_A9PLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg0_pll_refdiv,pll_refdiv,HI_CRG_A9PLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg0_pll_intprog,pll_intprog,HI_CRG_A9PLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg0_pll_postdiv1,pll_postdiv1,HI_CRG_A9PLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg0_pll_postdiv2,pll_postdiv2,HI_CRG_A9PLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg0_pll_lock,pll_lock,HI_CRG_A9PLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg0_reserved,reserved,HI_CRG_A9PLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg1_pll_fracdiv,pll_fracdiv,HI_CRG_A9PLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg1_pll_int_mod,pll_int_mod,HI_CRG_A9PLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg1_pll_cfg_vld,pll_cfg_vld,HI_CRG_A9PLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg1_pll_clk_gt,pll_clk_gt,HI_CRG_A9PLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg1_reserved,reserved,HI_CRG_A9PLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg2_pll_ssc_reset,pll_ssc_reset,HI_CRG_A9PLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg2_pll_ssc_disable,pll_ssc_disable,HI_CRG_A9PLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg2_pll_ssc_downspread,pll_ssc_downspread,HI_CRG_A9PLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg2_pll_ssc_spread,pll_ssc_spread,HI_CRG_A9PLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg2_pll_ssc_divval,pll_ssc_divval,HI_CRG_A9PLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_a9pll_cfg2_reserved,reserved,HI_CRG_A9PLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_A9PLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg0_pll_en,pll_en,HI_CRG_DSPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg0_pll_bp,pll_bp,HI_CRG_DSPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg0_pll_refdiv,pll_refdiv,HI_CRG_DSPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg0_pll_intprog,pll_intprog,HI_CRG_DSPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg0_pll_postdiv1,pll_postdiv1,HI_CRG_DSPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg0_pll_postdiv2,pll_postdiv2,HI_CRG_DSPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg0_pll_lock,pll_lock,HI_CRG_DSPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg0_reserved,reserved,HI_CRG_DSPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg1_pll_fracdiv,pll_fracdiv,HI_CRG_DSPPLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg1_pll_int_mod,pll_int_mod,HI_CRG_DSPPLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg1_pll_cfg_vld,pll_cfg_vld,HI_CRG_DSPPLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg1_pll_clk_gt,pll_clk_gt,HI_CRG_DSPPLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg1_reserved,reserved,HI_CRG_DSPPLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg2_pll_ssc_reset,pll_ssc_reset,HI_CRG_DSPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg2_pll_ssc_disable,pll_ssc_disable,HI_CRG_DSPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg2_pll_ssc_downspread,pll_ssc_downspread,HI_CRG_DSPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg2_pll_ssc_spread,pll_ssc_spread,HI_CRG_DSPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg2_pll_ssc_divval,pll_ssc_divval,HI_CRG_DSPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_dsppll_cfg2_reserved,reserved,HI_CRG_DSPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_DSPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg0_pll_en,pll_en,HI_CRG_BBPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg0_pll_bp,pll_bp,HI_CRG_BBPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg0_pll_refdiv,pll_refdiv,HI_CRG_BBPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg0_pll_intprog,pll_intprog,HI_CRG_BBPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg0_pll_postdiv1,pll_postdiv1,HI_CRG_BBPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg0_pll_postdiv2,pll_postdiv2,HI_CRG_BBPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg0_pll_lock,pll_lock,HI_CRG_BBPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg0_reserved,reserved,HI_CRG_BBPPLL_CFG0_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG0_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg1_pll_fracdiv,pll_fracdiv,HI_CRG_BBPPLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg1_pll_int_mod,pll_int_mod,HI_CRG_BBPPLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg1_pll_cfg_vld,pll_cfg_vld,HI_CRG_BBPPLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg1_pll_clk_gt,pll_clk_gt,HI_CRG_BBPPLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg1_reserved,reserved,HI_CRG_BBPPLL_CFG1_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG1_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg2_pll_ssc_reset,pll_ssc_reset,HI_CRG_BBPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg2_pll_ssc_disable,pll_ssc_disable,HI_CRG_BBPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg2_pll_ssc_downspread,pll_ssc_downspread,HI_CRG_BBPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg2_pll_ssc_spread,pll_ssc_spread,HI_CRG_BBPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg2_pll_ssc_divval,pll_ssc_divval,HI_CRG_BBPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG2_OFFSET)
HI_SET_GET(hi_crg_bbppll_cfg2_reserved,reserved,HI_CRG_BBPPLL_CFG2_T,HI_SYSSC_BASE_ADDR, HI_CRG_BBPPLL_CFG2_OFFSET)
HI_SET_GET(hi_sc_ctrl0_mcpu_boot_remap_clear,mcpu_boot_remap_clear,HI_SC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL0_OFFSET)
HI_SET_GET(hi_sc_ctrl0_reserved,reserved,HI_SC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL0_OFFSET)
HI_SET_GET(hi_sc_ctrl2_wdt_clk_en,wdt_clk_en,HI_SC_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL2_OFFSET)
HI_SET_GET(hi_sc_ctrl2_reserved_1,reserved_1,HI_SC_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL2_OFFSET)
HI_SET_GET(hi_sc_ctrl2_wdt_en_ov,wdt_en_ov,HI_SC_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL2_OFFSET)
HI_SET_GET(hi_sc_ctrl2_reserved_0,reserved_0,HI_SC_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL2_OFFSET)
HI_SET_GET(hi_sc_ctrl2_wdt_en_ctrl,wdt_en_ctrl,HI_SC_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL2_OFFSET)
HI_SET_GET(hi_sc_ctrl3_reserved_3,reserved_3,HI_SC_CTRL3_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL3_OFFSET)
HI_SET_GET(hi_sc_ctrl3_amon_monitor_start,amon_monitor_start,HI_SC_CTRL3_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL3_OFFSET)
HI_SET_GET(hi_sc_ctrl3_reserved_2,reserved_2,HI_SC_CTRL3_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL3_OFFSET)
HI_SET_GET(hi_sc_ctrl3_dsp0_uart_en,dsp0_uart_en,HI_SC_CTRL3_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL3_OFFSET)
HI_SET_GET(hi_sc_ctrl3_reserved_1,reserved_1,HI_SC_CTRL3_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL3_OFFSET)
HI_SET_GET(hi_sc_ctrl3_reserved_0,reserved_0,HI_SC_CTRL3_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL3_OFFSET)
HI_SET_GET(hi_sc_ctrl4_cicom0_sel_mod,cicom0_sel_mod,HI_SC_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL4_OFFSET)
HI_SET_GET(hi_sc_ctrl4_cicom1_sel_mod,cicom1_sel_mod,HI_SC_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL4_OFFSET)
HI_SET_GET(hi_sc_ctrl4_reserved,reserved,HI_SC_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL4_OFFSET)
HI_SET_GET(hi_sc_ctrl5_reserved,reserved,HI_SC_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL5_OFFSET)
HI_SET_GET(hi_sc_ctrl6_reserved,reserved,HI_SC_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL6_OFFSET)
HI_SET_GET(hi_sc_ctrl9_reserved,reserved,HI_SC_CTRL9_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL9_OFFSET)
HI_SET_GET(hi_sc_ctrl9_sw_dsp0_boot_addr,sw_dsp0_boot_addr,HI_SC_CTRL9_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL9_OFFSET)
HI_SET_GET(hi_sc_ctrl10_reserved,reserved,HI_SC_CTRL10_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL10_OFFSET)
HI_SET_GET(hi_sc_ctrl11_reserved_2,reserved_2,HI_SC_CTRL11_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL11_OFFSET)
HI_SET_GET(hi_sc_ctrl11_moda9_cfgnmfi,moda9_cfgnmfi,HI_SC_CTRL11_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL11_OFFSET)
HI_SET_GET(hi_sc_ctrl11_moda9_cfgsdisable,moda9_cfgsdisable,HI_SC_CTRL11_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL11_OFFSET)
HI_SET_GET(hi_sc_ctrl11_moda9_cp15sdisable,moda9_cp15sdisable,HI_SC_CTRL11_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL11_OFFSET)
HI_SET_GET(hi_sc_ctrl11_moda9_pwrctli0,moda9_pwrctli0,HI_SC_CTRL11_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL11_OFFSET)
HI_SET_GET(hi_sc_ctrl11_reserved_1,reserved_1,HI_SC_CTRL11_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL11_OFFSET)
HI_SET_GET(hi_sc_ctrl11_moda9_l2_waysize,moda9_l2_waysize,HI_SC_CTRL11_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL11_OFFSET)
HI_SET_GET(hi_sc_ctrl11_reserved_0,reserved_0,HI_SC_CTRL11_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL11_OFFSET)
HI_SET_GET(hi_sc_ctrl11_moda9_l2_regfilebase,moda9_l2_regfilebase,HI_SC_CTRL11_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL11_OFFSET)
HI_SET_GET(hi_sc_ctrl12_reserved,reserved,HI_SC_CTRL12_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL12_OFFSET)
HI_SET_GET(hi_sc_ctrl13_reserved_1,reserved_1,HI_SC_CTRL13_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL13_OFFSET)
HI_SET_GET(hi_sc_ctrl13_ocdhaltonreset,ocdhaltonreset,HI_SC_CTRL13_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL13_OFFSET)
HI_SET_GET(hi_sc_ctrl13_runstall,runstall,HI_SC_CTRL13_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL13_OFFSET)
HI_SET_GET(hi_sc_ctrl13_statvectorsel,statvectorsel,HI_SC_CTRL13_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL13_OFFSET)
HI_SET_GET(hi_sc_ctrl13_breaksync_en,breaksync_en,HI_SC_CTRL13_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL13_OFFSET)
HI_SET_GET(hi_sc_ctrl13_crosstrig_en,crosstrig_en,HI_SC_CTRL13_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL13_OFFSET)
HI_SET_GET(hi_sc_ctrl13_reserved_0,reserved_0,HI_SC_CTRL13_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL13_OFFSET)
HI_SET_GET(hi_sc_ctrl17_hpm_clk_div,hpm_clk_div,HI_SC_CTRL17_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL17_OFFSET)
HI_SET_GET(hi_sc_ctrl17_reserved_1,reserved_1,HI_SC_CTRL17_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL17_OFFSET)
HI_SET_GET(hi_sc_ctrl17_hpm_en,hpm_en,HI_SC_CTRL17_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL17_OFFSET)
HI_SET_GET(hi_sc_ctrl17_hpmx_en,hpmx_en,HI_SC_CTRL17_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL17_OFFSET)
HI_SET_GET(hi_sc_ctrl17_reserved_0,reserved_0,HI_SC_CTRL17_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL17_OFFSET)
HI_SET_GET(hi_sc_ctrl20_axi_mem_gatedclock_en,axi_mem_gatedclock_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_cicom0_auto_clk_gate_en,cicom0_auto_clk_gate_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_cicom0_soft_gate_clk_en,cicom0_soft_gate_clk_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_cicom1_auto_clk_gate_en,cicom1_auto_clk_gate_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_cicom1_soft_gate_clk_en,cicom1_soft_gate_clk_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_hs_uart_gatedclock_en,hs_uart_gatedclock_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_uart_gatedclock_en,uart_gatedclock_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_reserved_2,reserved_2,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_uicc_gatedclock_en,uicc_gatedclock_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_uicc_ss_scaledown_mode,uicc_ss_scaledown_mode,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_upacc_auto_clk_gate_en,upacc_auto_clk_gate_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_upacc_soft_gate_clk_en,upacc_soft_gate_clk_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_bbe16_cg_en,bbe16_cg_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_reserved_1,reserved_1,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_ipcm_auto_clk_gate_en,ipcm_auto_clk_gate_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_ipcm_soft_gate_clk_en,ipcm_soft_gate_clk_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_reserved_0,reserved_0,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl20_sysreg_auto_gate_en,sysreg_auto_gate_en,HI_SC_CTRL20_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL20_OFFSET)
HI_SET_GET(hi_sc_ctrl21_reserved_6,reserved_6,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_timer_gatedclock_en,timer_gatedclock_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_reserved_5,reserved_5,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_dw_axi_glb_cg_en,dw_axi_glb_cg_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_dw_axi_mst_cg_en,dw_axi_mst_cg_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_reserved_4,reserved_4,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_dw_axi_bbphy_cg_en,dw_axi_bbphy_cg_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_reserved_3,reserved_3,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_dw_x2x_async_cg_en,dw_x2x_async_cg_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_reserved_2,reserved_2,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_dw_x2h_qsync_cg_en,dw_x2h_qsync_cg_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_dw_hmx_cg_en,dw_hmx_cg_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_dw_x2p_cg_en,dw_x2p_cg_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_dw_gs_cg_en,dw_gs_cg_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_ashb_gatedclock_en,ashb_gatedclock_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_dw_ahb_mst_gatedclock_en,dw_ahb_mst_gatedclock_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_reserved_1,reserved_1,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_edmac_autogated_clk_en,edmac_autogated_clk_en,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl21_reserved_0,reserved_0,HI_SC_CTRL21_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL21_OFFSET)
HI_SET_GET(hi_sc_ctrl22_reserved_1,reserved_1,HI_SC_CTRL22_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL22_OFFSET)
HI_SET_GET(hi_sc_ctrl22_bbe16_ema,bbe16_ema,HI_SC_CTRL22_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL22_OFFSET)
HI_SET_GET(hi_sc_ctrl22_bbe16_emaw,bbe16_emaw,HI_SC_CTRL22_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL22_OFFSET)
HI_SET_GET(hi_sc_ctrl22_bbe16_emas,bbe16_emas,HI_SC_CTRL22_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL22_OFFSET)
HI_SET_GET(hi_sc_ctrl22_reserved_0,reserved_0,HI_SC_CTRL22_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL22_OFFSET)
HI_SET_GET(hi_sc_ctrl23_axi_mem_ema,axi_mem_ema,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_axi_mem_emaw,axi_mem_emaw,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_axi_mem_emas,axi_mem_emas,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_reserved_1,reserved_1,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_soc_mem_ema,soc_mem_ema,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_soc_mem_spram_emaw,soc_mem_spram_emaw,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_soc_mem_spram_emas,soc_mem_spram_emas,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_soc_mem_tpram_emab,soc_mem_tpram_emab,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_soc_mem_tpram_emasa,soc_mem_tpram_emasa,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_soc_mem_dpram_emaw,soc_mem_dpram_emaw,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_soc_mem_dpram_emas,soc_mem_dpram_emas,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl23_reserved_0,reserved_0,HI_SC_CTRL23_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL23_OFFSET)
HI_SET_GET(hi_sc_ctrl24_reserved_1,reserved_1,HI_SC_CTRL24_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL24_OFFSET)
HI_SET_GET(hi_sc_ctrl24_moda9_hs_mem_adjust,moda9_hs_mem_adjust,HI_SC_CTRL24_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL24_OFFSET)
HI_SET_GET(hi_sc_ctrl24_reserved_0,reserved_0,HI_SC_CTRL24_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL24_OFFSET)
HI_SET_GET(hi_sc_ctrl24_moda9_l2_ema,moda9_l2_ema,HI_SC_CTRL24_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL24_OFFSET)
HI_SET_GET(hi_sc_ctrl24_moda9_l2_emaw,moda9_l2_emaw,HI_SC_CTRL24_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL24_OFFSET)
HI_SET_GET(hi_sc_ctrl24_moda9_l2_emas,moda9_l2_emas,HI_SC_CTRL24_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL24_OFFSET)
HI_SET_GET(hi_sc_ctrl25_reserved,reserved,HI_SC_CTRL25_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL25_OFFSET)
HI_SET_GET(hi_sc_ctrl26_reserved,reserved,HI_SC_CTRL26_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL26_OFFSET)
HI_SET_GET(hi_sc_ctrl28_acp_filter_start,acp_filter_start,HI_SC_CTRL28_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL28_OFFSET)
HI_SET_GET(hi_sc_ctrl29_acp_filter_end,acp_filter_end,HI_SC_CTRL29_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL29_OFFSET)
HI_SET_GET(hi_sc_ctrl30_reserved,reserved,HI_SC_CTRL30_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL30_OFFSET)
HI_SET_GET(hi_sc_ctrl45_uicc_ic_usb_vbusvalid,uicc_ic_usb_vbusvalid,HI_SC_CTRL45_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL45_OFFSET)
HI_SET_GET(hi_sc_ctrl45_reserved,reserved,HI_SC_CTRL45_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL45_OFFSET)
HI_SET_GET(hi_sc_ctrl52_dsp0_nmi,dsp0_nmi,HI_SC_CTRL52_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL52_OFFSET)
HI_SET_GET(hi_sc_ctrl52_reserved,reserved,HI_SC_CTRL52_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL52_OFFSET)
HI_SET_GET(hi_sc_ctrl55_reserved_7,reserved_7,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_uicc_ret1n,uicc_ret1n,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_reserved_6,reserved_6,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_amon_soc_ret1n,amon_soc_ret1n,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_reserved_5,reserved_5,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_edmac_colldisn,edmac_colldisn,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_reserved_4,reserved_4,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_amon_soc_colldisn,amon_soc_colldisn,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_reserved_3,reserved_3,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_ipf_ret1n,ipf_ret1n,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_ipf_colldisn,ipf_colldisn,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_cipher_ret1n,cipher_ret1n,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_cipher_colldisn,cipher_colldisn,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_reserved_2,reserved_2,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_hs_uart_ret1n,hs_uart_ret1n,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_reserved_1,reserved_1,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_edmac_ret1n,edmac_ret1n,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl55_reserved_0,reserved_0,HI_SC_CTRL55_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL55_OFFSET)
HI_SET_GET(hi_sc_ctrl56_prior_level,prior_level,HI_SC_CTRL56_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL56_OFFSET)
HI_SET_GET(hi_sc_ctrl56_gatedclock_en,gatedclock_en,HI_SC_CTRL56_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL56_OFFSET)
HI_SET_GET(hi_sc_ctrl56_dw_axi_rs_gatedclock_en,dw_axi_rs_gatedclock_en,HI_SC_CTRL56_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL56_OFFSET)
HI_SET_GET(hi_sc_ctrl56_dw_axi_gs_gatedclock_en,dw_axi_gs_gatedclock_en,HI_SC_CTRL56_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL56_OFFSET)
HI_SET_GET(hi_sc_ctrl56_overf_prot,overf_prot,HI_SC_CTRL56_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL56_OFFSET)
HI_SET_GET(hi_sc_ctrl56_reserved,reserved,HI_SC_CTRL56_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL56_OFFSET)
HI_SET_GET(hi_sc_ctrl56_atpram_ctrl,atpram_ctrl,HI_SC_CTRL56_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL56_OFFSET)
HI_SET_GET(hi_sc_ctrl57_soc2lte_tbd,soc2lte_tbd,HI_SC_CTRL57_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL57_OFFSET)
HI_SET_GET(hi_sc_ctrl57_reserved,reserved,HI_SC_CTRL57_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL57_OFFSET)
HI_SET_GET(hi_sc_ctrl68_moda9_l2_ret1n,moda9_l2_ret1n,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_moda9_l2_ret2n,moda9_l2_ret2n,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_moda9_l2_pgen,moda9_l2_pgen,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_reserved_2,reserved_2,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_dsp0_ret1n,dsp0_ret1n,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_dsp0_ret2n,dsp0_ret2n,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_dsp0_pgen,dsp0_pgen,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_reserved_1,reserved_1,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_pd_axi_mem_ret1n,pd_axi_mem_ret1n,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_pd_axi_mem_ret2n,pd_axi_mem_ret2n,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_pd_axi_mem_pgen,pd_axi_mem_pgen,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl68_reserved_0,reserved_0,HI_SC_CTRL68_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL68_OFFSET)
HI_SET_GET(hi_sc_ctrl69_axi_bbphy_xdcdr_sel,axi_bbphy_xdcdr_sel,HI_SC_CTRL69_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL69_OFFSET)
HI_SET_GET(hi_sc_ctrl69_reserved_2,reserved_2,HI_SC_CTRL69_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL69_OFFSET)
HI_SET_GET(hi_sc_ctrl69_axi_glb_xdcdr_sel,axi_glb_xdcdr_sel,HI_SC_CTRL69_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL69_OFFSET)
HI_SET_GET(hi_sc_ctrl69_reserved_1,reserved_1,HI_SC_CTRL69_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL69_OFFSET)
HI_SET_GET(hi_sc_ctrl69_socapb_pslverr_sel,socapb_pslverr_sel,HI_SC_CTRL69_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL69_OFFSET)
HI_SET_GET(hi_sc_ctrl69_x2h_hslverr_sel,x2h_hslverr_sel,HI_SC_CTRL69_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL69_OFFSET)
HI_SET_GET(hi_sc_ctrl69_mst_err_srst_req,mst_err_srst_req,HI_SC_CTRL69_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL69_OFFSET)
HI_SET_GET(hi_sc_ctrl69_reserved_0,reserved_0,HI_SC_CTRL69_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL69_OFFSET)
HI_SET_GET(hi_sc_ctrl70_axi_bbphy_priority_m1,axi_bbphy_priority_m1,HI_SC_CTRL70_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL70_OFFSET)
HI_SET_GET(hi_sc_ctrl70_axi_bbphy_priority_m2,axi_bbphy_priority_m2,HI_SC_CTRL70_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL70_OFFSET)
HI_SET_GET(hi_sc_ctrl70_axi_bbphy_priority_m3,axi_bbphy_priority_m3,HI_SC_CTRL70_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL70_OFFSET)
HI_SET_GET(hi_sc_ctrl70_axi_bbphy_priority_m4,axi_bbphy_priority_m4,HI_SC_CTRL70_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL70_OFFSET)
HI_SET_GET(hi_sc_ctrl70_reserved_1,reserved_1,HI_SC_CTRL70_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL70_OFFSET)
HI_SET_GET(hi_sc_ctrl70_axi_mst_priority_m9,axi_mst_priority_m9,HI_SC_CTRL70_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL70_OFFSET)
HI_SET_GET(hi_sc_ctrl70_axi_mst_priority_m10,axi_mst_priority_m10,HI_SC_CTRL70_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL70_OFFSET)
HI_SET_GET(hi_sc_ctrl70_reserved_0,reserved_0,HI_SC_CTRL70_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL70_OFFSET)
HI_SET_GET(hi_sc_ctrl71_reserved,reserved,HI_SC_CTRL71_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL71_OFFSET)
HI_SET_GET(hi_sc_ctrl71_axi_glb_priority_m1,axi_glb_priority_m1,HI_SC_CTRL71_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL71_OFFSET)
HI_SET_GET(hi_sc_ctrl71_axi_glb_priority_m2,axi_glb_priority_m2,HI_SC_CTRL71_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL71_OFFSET)
HI_SET_GET(hi_sc_ctrl71_axi_glb_priority_m3,axi_glb_priority_m3,HI_SC_CTRL71_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL71_OFFSET)
HI_SET_GET(hi_sc_ctrl71_axi_glb_priority_m4,axi_glb_priority_m4,HI_SC_CTRL71_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL71_OFFSET)
HI_SET_GET(hi_sc_ctrl71_axi_glb_priority_m5,axi_glb_priority_m5,HI_SC_CTRL71_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL71_OFFSET)
HI_SET_GET(hi_sc_ctrl71_axi_glb_priority_m6,axi_glb_priority_m6,HI_SC_CTRL71_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL71_OFFSET)
HI_SET_GET(hi_sc_ctrl71_axi_mst_cache_cfg_en,axi_mst_cache_cfg_en,HI_SC_CTRL71_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL71_OFFSET)
HI_SET_GET(hi_sc_ctrl71_axi_mst_sideband,axi_mst_sideband,HI_SC_CTRL71_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL71_OFFSET)
HI_SET_GET(hi_sc_ctrl71_axi_mst_cache,axi_mst_cache,HI_SC_CTRL71_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL71_OFFSET)
HI_SET_GET(hi_sc_ctrl72_axi_mst_priority_m1,axi_mst_priority_m1,HI_SC_CTRL72_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL72_OFFSET)
HI_SET_GET(hi_sc_ctrl72_axi_mst_priority_m2,axi_mst_priority_m2,HI_SC_CTRL72_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL72_OFFSET)
HI_SET_GET(hi_sc_ctrl72_axi_mst_priority_m3,axi_mst_priority_m3,HI_SC_CTRL72_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL72_OFFSET)
HI_SET_GET(hi_sc_ctrl72_axi_mst_priority_m4,axi_mst_priority_m4,HI_SC_CTRL72_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL72_OFFSET)
HI_SET_GET(hi_sc_ctrl72_axi_mst_priority_m5,axi_mst_priority_m5,HI_SC_CTRL72_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL72_OFFSET)
HI_SET_GET(hi_sc_ctrl72_axi_mst_priority_m6,axi_mst_priority_m6,HI_SC_CTRL72_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL72_OFFSET)
HI_SET_GET(hi_sc_ctrl72_axi_mst_priority_m7,axi_mst_priority_m7,HI_SC_CTRL72_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL72_OFFSET)
HI_SET_GET(hi_sc_ctrl72_axi_mst_priority_m8,axi_mst_priority_m8,HI_SC_CTRL72_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL72_OFFSET)
HI_SET_GET(hi_sc_ctrl73_peri_mst_region1_ctrl,peri_mst_region1_ctrl,HI_SC_CTRL73_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL73_OFFSET)
HI_SET_GET(hi_sc_ctrl73_peri_mst_secctrl_bypass,peri_mst_secctrl_bypass,HI_SC_CTRL73_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL73_OFFSET)
HI_SET_GET(hi_sc_ctrl73_reserved,reserved,HI_SC_CTRL73_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL73_OFFSET)
HI_SET_GET(hi_sc_ctrl73_peri_mst_region1_staddr,peri_mst_region1_staddr,HI_SC_CTRL73_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL73_OFFSET)
HI_SET_GET(hi_sc_ctrl74_reserved,reserved,HI_SC_CTRL74_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL74_OFFSET)
HI_SET_GET(hi_sc_ctrl74_peri_mst_region1_endaddr,peri_mst_region1_endaddr,HI_SC_CTRL74_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL74_OFFSET)
HI_SET_GET(hi_sc_ctrl75_peri_mst_region2_ctrl,peri_mst_region2_ctrl,HI_SC_CTRL75_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL75_OFFSET)
HI_SET_GET(hi_sc_ctrl75_reserved,reserved,HI_SC_CTRL75_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL75_OFFSET)
HI_SET_GET(hi_sc_ctrl75_peri_mst_region2_staddr,peri_mst_region2_staddr,HI_SC_CTRL75_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL75_OFFSET)
HI_SET_GET(hi_sc_ctrl76_reserved,reserved,HI_SC_CTRL76_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL76_OFFSET)
HI_SET_GET(hi_sc_ctrl76_peri_mst_region2_endaddr,peri_mst_region2_endaddr,HI_SC_CTRL76_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL76_OFFSET)
HI_SET_GET(hi_sc_ctrl77_peri_mst_region3_ctrl,peri_mst_region3_ctrl,HI_SC_CTRL77_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL77_OFFSET)
HI_SET_GET(hi_sc_ctrl77_reserved,reserved,HI_SC_CTRL77_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL77_OFFSET)
HI_SET_GET(hi_sc_ctrl77_peri_mst_region3_staddr,peri_mst_region3_staddr,HI_SC_CTRL77_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL77_OFFSET)
HI_SET_GET(hi_sc_ctrl78_reserved,reserved,HI_SC_CTRL78_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL78_OFFSET)
HI_SET_GET(hi_sc_ctrl78_peri_mst_region3_endaddr,peri_mst_region3_endaddr,HI_SC_CTRL78_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL78_OFFSET)
HI_SET_GET(hi_sc_ctrl79_peri_mst_region4_ctrl,peri_mst_region4_ctrl,HI_SC_CTRL79_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL79_OFFSET)
HI_SET_GET(hi_sc_ctrl79_reserved,reserved,HI_SC_CTRL79_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL79_OFFSET)
HI_SET_GET(hi_sc_ctrl79_peri_mst_region4_staddr,peri_mst_region4_staddr,HI_SC_CTRL79_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL79_OFFSET)
HI_SET_GET(hi_sc_ctrl80_reserved,reserved,HI_SC_CTRL80_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL80_OFFSET)
HI_SET_GET(hi_sc_ctrl80_peri_mst_region4_endaddr,peri_mst_region4_endaddr,HI_SC_CTRL80_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL80_OFFSET)
HI_SET_GET(hi_sc_ctrl103_reserved_8,reserved_8,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_bbphy_dsp0_axislv_active_mask,bbphy_dsp0_axislv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_bbphy_bbp_axislv_active_mask,bbphy_bbp_axislv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_reserved_7,reserved_7,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_reserved_6,reserved_6,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_mdmacp_axislv_active_mask,mdmacp_axislv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_reserved_5,reserved_5,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_axi_mem_axislv_active_mask,axi_mem_axislv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_reserved_4,reserved_4,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_bbphy_axislv_active_mask,bbphy_axislv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_reserved_3,reserved_3,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_wbbp_axislv_active_mask,wbbp_axislv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_amon_axislv_active_mask,amon_axislv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_reserved_2,reserved_2,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_wdog_apbslv_active_mask,wdog_apbslv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_timer0_7_apbslv_active_mask,timer0_7_apbslv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_reserved_1,reserved_1,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_uart0_apbslv_active_mask,uart0_apbslv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_edmac_slv_active_mask,edmac_slv_active_mask,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl103_reserved_0,reserved_0,HI_SC_CTRL103_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL103_OFFSET)
HI_SET_GET(hi_sc_ctrl104_reserved,reserved,HI_SC_CTRL104_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL104_OFFSET)
HI_SET_GET(hi_sc_ctrl105_uicc,uicc,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_reserved_6,reserved_6,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_ipcm,ipcm,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_reserved_5,reserved_5,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_hs_uart,hs_uart,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_reserved_4,reserved_4,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_cipher,cipher,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_ipf,ipf,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_reserved_3,reserved_3,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_cicom0,cicom0,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_cicom1,cicom1,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_reserved_2,reserved_2,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_vic1,vic1,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_reserved_1,reserved_1,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_upacc,upacc,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_ctrl105_reserved_0,reserved_0,HI_SC_CTRL105_T,HI_SYSSC_BASE_ADDR, HI_SC_CTRL105_OFFSET)
HI_SET_GET(hi_sc_stat1_reserved_1,reserved_1,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_standbywfi,moda9_standbywfi,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_standbywfe,moda9_standbywfe,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_pmupriv,moda9_pmupriv,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_pmusecure,moda9_pmusecure,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_smpnamp,moda9_smpnamp,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_scuevabort,moda9_scuevabort,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_pwrctlo0,moda9_pwrctlo0,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_l2_tagclkouten,moda9_l2_tagclkouten,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_l2_dataclkouten,moda9_l2_dataclkouten,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_l2_idle,moda9_l2_idle,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_moda9_l2_clkstopped,moda9_l2_clkstopped,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat1_reserved_0,reserved_0,HI_SC_STAT1_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT1_OFFSET)
HI_SET_GET(hi_sc_stat2_reserved_1,reserved_1,HI_SC_STAT2_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT2_OFFSET)
HI_SET_GET(hi_sc_stat2_dsp0_pwaitmode,dsp0_pwaitmode,HI_SC_STAT2_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT2_OFFSET)
HI_SET_GET(hi_sc_stat2_dsp0_xocdmode,dsp0_xocdmode,HI_SC_STAT2_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT2_OFFSET)
HI_SET_GET(hi_sc_stat2_reserved_0,reserved_0,HI_SC_STAT2_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT2_OFFSET)
HI_SET_GET(hi_sc_stat3_hpmx_opc,hpmx_opc,HI_SC_STAT3_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT3_OFFSET)
HI_SET_GET(hi_sc_stat3_hpmx_opc_vld,hpmx_opc_vld,HI_SC_STAT3_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT3_OFFSET)
HI_SET_GET(hi_sc_stat3_reserved_1,reserved_1,HI_SC_STAT3_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT3_OFFSET)
HI_SET_GET(hi_sc_stat3_hpm_opc,hpm_opc,HI_SC_STAT3_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT3_OFFSET)
HI_SET_GET(hi_sc_stat3_hpm_opc_vld,hpm_opc_vld,HI_SC_STAT3_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT3_OFFSET)
HI_SET_GET(hi_sc_stat3_reserved_0,reserved_0,HI_SC_STAT3_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT3_OFFSET)
HI_SET_GET(hi_sc_stat5_dsp0_nmi,dsp0_nmi,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_intr_ipc_ns,intr_ipc_ns,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_intr_ipc_ns_mbx,intr_ipc_ns_mbx,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_dsp_uart_int,dsp_uart_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_edmac_int1,edmac_int1,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_lte_dsp_aagc_int,lte_dsp_aagc_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_lte_dsp_cell_int,lte_dsp_cell_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_lte_dsp_vdl_int,lte_dsp_vdl_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_lte_dsp_cmu_int,lte_dsp_cmu_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_lte_dsp_pwrm_int,lte_dsp_pwrm_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_lte_dsp_cfi_int,lte_dsp_cfi_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_lte_dsp_tdl_int,lte_dsp_tdl_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_lte_dsp_bbp_dma_int,lte_dsp_bbp_dma_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_lte_dsp_pub_int,lte_dsp_pub_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_lte_dsp_synb_int,lte_dsp_synb_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_tds_stu_sfrm_int,tds_stu_sfrm_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_tds_hsupa_int,tds_hsupa_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_tds_harq_int,tds_harq_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_tds_turbo_int,tds_turbo_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_tds_viterbi_int,tds_viterbi_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_tds_rfc_int,tds_rfc_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_tds_fpu_int,tds_fpu_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_tds_stu_dsp_int,tds_stu_dsp_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_ctu_int_lte,ctu_int_lte,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_ctu_int_tds,ctu_int_tds,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_dsp_vic_int,dsp_vic_int,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat5_reserved,reserved,HI_SC_STAT5_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT5_OFFSET)
HI_SET_GET(hi_sc_stat6_reserved_1,reserved_1,HI_SC_STAT6_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT6_OFFSET)
HI_SET_GET(hi_sc_stat6_moda9_parityfail0,moda9_parityfail0,HI_SC_STAT6_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT6_OFFSET)
HI_SET_GET(hi_sc_stat6_moda9_parityfailscu,moda9_parityfailscu,HI_SC_STAT6_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT6_OFFSET)
HI_SET_GET(hi_sc_stat6_reserved_0,reserved_0,HI_SC_STAT6_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT6_OFFSET)
HI_SET_GET(hi_sc_stat9_dw_axi_mst_dlock_wr,dw_axi_mst_dlock_wr,HI_SC_STAT9_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT9_OFFSET)
HI_SET_GET(hi_sc_stat9_dw_axi_mst_dlock_slv,dw_axi_mst_dlock_slv,HI_SC_STAT9_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT9_OFFSET)
HI_SET_GET(hi_sc_stat9_reserved_2,reserved_2,HI_SC_STAT9_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT9_OFFSET)
HI_SET_GET(hi_sc_stat9_dw_axi_mst_dlock_mst,dw_axi_mst_dlock_mst,HI_SC_STAT9_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT9_OFFSET)
HI_SET_GET(hi_sc_stat9_reserved_1,reserved_1,HI_SC_STAT9_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT9_OFFSET)
HI_SET_GET(hi_sc_stat9_dw_axi_glb_dlock_wr,dw_axi_glb_dlock_wr,HI_SC_STAT9_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT9_OFFSET)
HI_SET_GET(hi_sc_stat9_dw_axi_glb_dlock_slv,dw_axi_glb_dlock_slv,HI_SC_STAT9_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT9_OFFSET)
HI_SET_GET(hi_sc_stat9_dw_axi_glb_dlock_mst,dw_axi_glb_dlock_mst,HI_SC_STAT9_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT9_OFFSET)
HI_SET_GET(hi_sc_stat9_reserved_0,reserved_0,HI_SC_STAT9_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT9_OFFSET)
HI_SET_GET(hi_sc_stat10_dw_axi_glb_dlock_id,dw_axi_glb_dlock_id,HI_SC_STAT10_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT10_OFFSET)
HI_SET_GET(hi_sc_stat10_reserved,reserved,HI_SC_STAT10_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT10_OFFSET)
HI_SET_GET(hi_sc_stat10_dw_axi_mst_dlock_id,dw_axi_mst_dlock_id,HI_SC_STAT10_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT10_OFFSET)
HI_SET_GET(hi_sc_stat15_apb_pslv_active0,apb_pslv_active0,HI_SC_STAT15_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT15_OFFSET)
HI_SET_GET(hi_sc_stat15_reserved,reserved,HI_SC_STAT15_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT15_OFFSET)
HI_SET_GET(hi_sc_stat22_dw_axi_bbphy_dlock_mst,dw_axi_bbphy_dlock_mst,HI_SC_STAT22_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT22_OFFSET)
HI_SET_GET(hi_sc_stat22_dw_axi_bbphy_dlock_slv,dw_axi_bbphy_dlock_slv,HI_SC_STAT22_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT22_OFFSET)
HI_SET_GET(hi_sc_stat22_dw_axi_bbphy_dlock_wr,dw_axi_bbphy_dlock_wr,HI_SC_STAT22_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT22_OFFSET)
HI_SET_GET(hi_sc_stat22_reserved_1,reserved_1,HI_SC_STAT22_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT22_OFFSET)
HI_SET_GET(hi_sc_stat22_dw_axi_bbphy_dlock_id,dw_axi_bbphy_dlock_id,HI_SC_STAT22_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT22_OFFSET)
HI_SET_GET(hi_sc_stat22_reserved_0,reserved_0,HI_SC_STAT22_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT22_OFFSET)
HI_SET_GET(hi_sc_stat26_reserved_1,reserved_1,HI_SC_STAT26_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT26_OFFSET)
HI_SET_GET(hi_sc_stat26_ipf_idle,ipf_idle,HI_SC_STAT26_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT26_OFFSET)
HI_SET_GET(hi_sc_stat26_cicom0_clk_state,cicom0_clk_state,HI_SC_STAT26_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT26_OFFSET)
HI_SET_GET(hi_sc_stat26_cicom1_clk_state,cicom1_clk_state,HI_SC_STAT26_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT26_OFFSET)
HI_SET_GET(hi_sc_stat26_reserved_0,reserved_0,HI_SC_STAT26_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT26_OFFSET)
HI_SET_GET(hi_sc_stat27_bbphy_slv_active,bbphy_slv_active,HI_SC_STAT27_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT27_OFFSET)
HI_SET_GET(hi_sc_stat27_reserved_1,reserved_1,HI_SC_STAT27_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT27_OFFSET)
HI_SET_GET(hi_sc_stat27_glb_slv_active,glb_slv_active,HI_SC_STAT27_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT27_OFFSET)
HI_SET_GET(hi_sc_stat27_reserved_0,reserved_0,HI_SC_STAT27_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT27_OFFSET)
HI_SET_GET(hi_sc_stat29_x2h_peri_slv_active,x2h_peri_slv_active,HI_SC_STAT29_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT29_OFFSET)
HI_SET_GET(hi_sc_stat32_lte2soc_tbd,lte2soc_tbd,HI_SC_STAT32_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT32_OFFSET)
HI_SET_GET(hi_sc_stat32_reserved,reserved,HI_SC_STAT32_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT32_OFFSET)
HI_SET_GET(hi_sc_stat35_ap2mdm_key0,ap2mdm_key0,HI_SC_STAT35_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT35_OFFSET)
HI_SET_GET(hi_sc_stat36_ap2mdm_key0,ap2mdm_key0,HI_SC_STAT36_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT36_OFFSET)
HI_SET_GET(hi_sc_stat37_ap2mdm_key2,ap2mdm_key2,HI_SC_STAT37_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT37_OFFSET)
HI_SET_GET(hi_sc_stat38_ap2mdm_key3,ap2mdm_key3,HI_SC_STAT38_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT38_OFFSET)
HI_SET_GET(hi_sc_stat41_bbphy_mst_err,bbphy_mst_err,HI_SC_STAT41_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT41_OFFSET)
HI_SET_GET(hi_sc_stat41_reserved,reserved,HI_SC_STAT41_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT41_OFFSET)
HI_SET_GET(hi_sc_stat41_glb_mst_err,glb_mst_err,HI_SC_STAT41_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT41_OFFSET)
HI_SET_GET(hi_sc_stat42_reserved_1,reserved_1,HI_SC_STAT42_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT42_OFFSET)
HI_SET_GET(hi_sc_stat42_socapb_psel_err,socapb_psel_err,HI_SC_STAT42_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT42_OFFSET)
HI_SET_GET(hi_sc_stat42_ahb_peri_mst_err,ahb_peri_mst_err,HI_SC_STAT42_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT42_OFFSET)
HI_SET_GET(hi_sc_stat42_reserved_0,reserved_0,HI_SC_STAT42_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT42_OFFSET)
HI_SET_GET(hi_sc_stat43_x2h_peri_addr_err,x2h_peri_addr_err,HI_SC_STAT43_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT43_OFFSET)
HI_SET_GET(hi_sc_stat44_reserved,reserved,HI_SC_STAT44_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT44_OFFSET)
HI_SET_GET(hi_sc_stat44_addr_err,addr_err,HI_SC_STAT44_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT44_OFFSET)
HI_SET_GET(hi_sc_stat46_addr_err,addr_err,HI_SC_STAT46_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT46_OFFSET)
HI_SET_GET(hi_sc_stat47_addr_err,addr_err,HI_SC_STAT47_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT47_OFFSET)
HI_SET_GET(hi_sc_stat48_addr_err,addr_err,HI_SC_STAT48_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT48_OFFSET)
HI_SET_GET(hi_sc_stat49_addr_err,addr_err,HI_SC_STAT49_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT49_OFFSET)
HI_SET_GET(hi_sc_stat50_addr_err,addr_err,HI_SC_STAT50_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT50_OFFSET)
HI_SET_GET(hi_sc_stat51_addr_err,addr_err,HI_SC_STAT51_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT51_OFFSET)
HI_SET_GET(hi_sc_stat52_addr_err,addr_err,HI_SC_STAT52_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT52_OFFSET)
HI_SET_GET(hi_sc_stat53_addr_err,addr_err,HI_SC_STAT53_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT53_OFFSET)
HI_SET_GET(hi_sc_stat54_addr_err,addr_err,HI_SC_STAT54_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT54_OFFSET)
HI_SET_GET(hi_sc_stat55_addr_err,addr_err,HI_SC_STAT55_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT55_OFFSET)
HI_SET_GET(hi_sc_stat56_addr_err,addr_err,HI_SC_STAT56_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT56_OFFSET)
HI_SET_GET(hi_sc_stat57_addr_err,addr_err,HI_SC_STAT57_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT57_OFFSET)
HI_SET_GET(hi_sc_stat62_addr_err,addr_err,HI_SC_STAT62_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT62_OFFSET)
HI_SET_GET(hi_sc_stat63_addr_err,addr_err,HI_SC_STAT63_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT63_OFFSET)
HI_SET_GET(hi_sc_stat64_addr_err,addr_err,HI_SC_STAT64_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT64_OFFSET)
HI_SET_GET(hi_sc_stat65_addr_err,addr_err,HI_SC_STAT65_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT65_OFFSET)
HI_SET_GET(hi_sc_stat66_addr_err,addr_err,HI_SC_STAT66_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT66_OFFSET)
HI_SET_GET(hi_sc_stat67_addr_err,addr_err,HI_SC_STAT67_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT67_OFFSET)
HI_SET_GET(hi_sc_stat68_addr_err,addr_err,HI_SC_STAT68_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT68_OFFSET)
HI_SET_GET(hi_sc_stat69_addr_err,addr_err,HI_SC_STAT69_T,HI_SYSSC_BASE_ADDR, HI_SC_STAT69_OFFSET)
HI_SET_GET(hi_pwr_ctrl2_reserved_1,reserved_1,HI_PWR_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL2_OFFSET)
HI_SET_GET(hi_pwr_ctrl2_sc_tcxo0_en_ctrl,sc_tcxo0_en_ctrl,HI_PWR_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL2_OFFSET)
HI_SET_GET(hi_pwr_ctrl2_sc_tcxo1_en_ctrl,sc_tcxo1_en_ctrl,HI_PWR_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL2_OFFSET)
HI_SET_GET(hi_pwr_ctrl2_reserved_0,reserved_0,HI_PWR_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL2_OFFSET)
HI_SET_GET(hi_pwr_ctrl2_sc_tcxo_en_ctrl,sc_tcxo_en_ctrl,HI_PWR_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL2_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_reserved_3,reserved_3,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_ccpu_iso_ctrl_en,ccpu_iso_ctrl_en,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_reserved_2,reserved_2,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_bbe16_iso_ctrl_en,bbe16_iso_ctrl_en,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_reserved_1,reserved_1,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_ltebbp_iso_ctrl_en,ltebbp_iso_ctrl_en,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_twbbp_iso_ctrl_en,twbbp_iso_ctrl_en,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_wbbp_iso_ctrl_en,wbbp_iso_ctrl_en,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_g1bbp_iso_ctrl_en,g1bbp_iso_ctrl_en,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_g2bbp_iso_ctrl_en,g2bbp_iso_ctrl_en,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_irmbbp_iso_ctrl_en,irmbbp_iso_ctrl_en,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl4_reserved_0,reserved_0,HI_PWR_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL4_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_reserved_3,reserved_3,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_ccpu_iso_ctrl_dis,ccpu_iso_ctrl_dis,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_reserved_2,reserved_2,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_bbe16_iso_ctrl_dis,bbe16_iso_ctrl_dis,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_reserved_1,reserved_1,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_ltebbp_iso_ctrl_dis,ltebbp_iso_ctrl_dis,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_twbbp_iso_ctrl_dis,twbbp_iso_ctrl_dis,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_wbbp_iso_ctrl_dis,wbbp_iso_ctrl_dis,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_g1bbp_iso_ctrl_dis,g1bbp_iso_ctrl_dis,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_g2bbp_iso_ctrl_dis,g2bbp_iso_ctrl_dis,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_irmbbp_iso_ctrl_dis,irmbbp_iso_ctrl_dis,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl5_reserved_0,reserved_0,HI_PWR_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL5_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_reserved_3,reserved_3,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_ccpu_mtcmos_ctrl_en,ccpu_mtcmos_ctrl_en,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_reserved_2,reserved_2,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_bbe16_mtcmos_ctrl_en,bbe16_mtcmos_ctrl_en,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_reserved_1,reserved_1,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_ltebbp_mtcmos_ctrl_en,ltebbp_mtcmos_ctrl_en,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_twbbp_mtcmos_ctrl_en,twbbp_mtcmos_ctrl_en,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_wbbp_mtcmos_ctrl_en,wbbp_mtcmos_ctrl_en,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_g1bbp_mtcmos_ctrl_en,g1bbp_mtcmos_ctrl_en,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_g2bbp_mtcmos_ctrl_en,g2bbp_mtcmos_ctrl_en,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_irmbbp_mtcmos_ctrl_en,irmbbp_mtcmos_ctrl_en,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl6_reserved_0,reserved_0,HI_PWR_CTRL6_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL6_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_reserved_3,reserved_3,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_ccpu_mtcmos_ctrl_dis,ccpu_mtcmos_ctrl_dis,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_reserved_2,reserved_2,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_bbe16_mtcmos_ctrl_dis,bbe16_mtcmos_ctrl_dis,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_reserved_1,reserved_1,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_ltebbp_mtcmos_ctrl_dis,ltebbp_mtcmos_ctrl_dis,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_twbbp_mtcmos_ctrl_dis,twbbp_mtcmos_ctrl_dis,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_wbbp_mtcmos_ctrl_dis,wbbp_mtcmos_ctrl_dis,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_g1bbp_mtcmos_ctrl_dis,g1bbp_mtcmos_ctrl_dis,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_g2bbp_mtcmos_ctrl_dis,g2bbp_mtcmos_ctrl_dis,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_irmbbp_mtcmos_ctrl_dis,irmbbp_mtcmos_ctrl_dis,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_ctrl7_reserved_0,reserved_0,HI_PWR_CTRL7_T,HI_SYSSC_BASE_ADDR, HI_PWR_CTRL7_OFFSET)
HI_SET_GET(hi_pwr_stat1_reserved_4,reserved_4,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_ccpu_mtcmos_rdy_stat,ccpu_mtcmos_rdy_stat,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_reserved_3,reserved_3,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_bbe16_mtcmos_rdy_stat,bbe16_mtcmos_rdy_stat,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_reserved_2,reserved_2,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_ltebbp_mtcmos_rdy_stat,ltebbp_mtcmos_rdy_stat,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_twbbp_mtcmos_rdy_stat,twbbp_mtcmos_rdy_stat,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_wbbp_mtcmos_rdy_stat,wbbp_mtcmos_rdy_stat,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_g1bbp_mtcmos_rdy_stat,g1bbp_mtcmos_rdy_stat,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_g2bbp_mtcmos_rdy_stat,g2bbp_mtcmos_rdy_stat,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_irmbbp_mtcmos_rdy_stat,irmbbp_mtcmos_rdy_stat,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_reserved_1,reserved_1,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_ltebbp0_mtcmos_rdy_stat,ltebbp0_mtcmos_rdy_stat,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat1_reserved_0,reserved_0,HI_PWR_STAT1_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT1_OFFSET)
HI_SET_GET(hi_pwr_stat3_reserved_3,reserved_3,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_ccpu_iso_ctrl_stat,ccpu_iso_ctrl_stat,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_reserved_2,reserved_2,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_bbe16_iso_ctrl_stat,bbe16_iso_ctrl_stat,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_reserved_1,reserved_1,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_ltebbp_iso_ctrl_stat,ltebbp_iso_ctrl_stat,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_twbbp_iso_ctrl_stat,twbbp_iso_ctrl_stat,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_wbbp_iso_ctrl_stat,wbbp_iso_ctrl_stat,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_g1bbp_iso_ctrl_stat,g1bbp_iso_ctrl_stat,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_g2bbp_iso_ctrl_stat,g2bbp_iso_ctrl_stat,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_irmbbp_iso_ctrl_stat,irmbbp_iso_ctrl_stat,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat3_reserved_0,reserved_0,HI_PWR_STAT3_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT3_OFFSET)
HI_SET_GET(hi_pwr_stat4_reserved_3,reserved_3,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_ccpu_mtcmos_ctrl_stat,ccpu_mtcmos_ctrl_stat,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_reserved_2,reserved_2,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_bbe16_mtcmos_ctrl_stat,bbe16_mtcmos_ctrl_stat,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_reserved_1,reserved_1,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_ltebbp_mtcmos_ctrl_stat,ltebbp_mtcmos_ctrl_stat,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_twbbp_mtcmos_ctrl_stat,twbbp_mtcmos_ctrl_stat,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_wbbp_mtcmos_ctrl_stat,wbbp_mtcmos_ctrl_stat,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_g1bbp_mtcmos_ctrl_stat,g1bbp_mtcmos_ctrl_stat,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_g2bbp_mtcmos_ctrl_stat,g2bbp_mtcmos_ctrl_stat,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_irmbbp_mtcmos_ctrl_stat,irmbbp_mtcmos_ctrl_stat,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_pwr_stat4_reserved_0,reserved_0,HI_PWR_STAT4_T,HI_SYSSC_BASE_ADDR, HI_PWR_STAT4_OFFSET)
HI_SET_GET(hi_sec_ctrl0_ipf_sec_w_ctrl,ipf_sec_w_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_ipf_sec_r_ctrl,ipf_sec_r_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_region1_sec_w_ctrl,region1_sec_w_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_region1_sec_r_ctrl,region1_sec_r_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_ipcm_sec_w_ctrl,ipcm_sec_w_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_ipcm_sec_r_ctrl,ipcm_sec_r_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_aximon_sec_w_ctrl,aximon_sec_w_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_aximon_sec_r_ctrl,aximon_sec_r_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_others_sec_w_ctrl,others_sec_w_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_others_sec_r_ctrl,others_sec_r_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_region2_sec_r_ctrl,region2_sec_r_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_uicc_sec_w_ctrl,uicc_sec_w_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_uicc_sec_r_ctrl,uicc_sec_r_ctrl,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl0_reserved,reserved,HI_SEC_CTRL0_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL0_OFFSET)
HI_SET_GET(hi_sec_ctrl1_noddr_bypass,noddr_bypass,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_reserved,reserved,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_sci0_sec_w_ctrl,sci0_sec_w_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_sci0_sec_r_ctrl,sci0_sec_r_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_sci1_sec_w_ctrl,sci1_sec_w_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_sci1_sec_r_ctrl,sci1_sec_r_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_ipcm_s_sec_w_ctrl,ipcm_s_sec_w_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_ipcm_s_sec_r_ctrl,ipcm_s_sec_r_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_ipcm_ns_sec_w_ctrl,ipcm_ns_sec_w_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_ipcm_ns_sec_r_ctrl,ipcm_ns_sec_r_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_secram_sec_w_ctrl,secram_sec_w_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_secram_sec_r_ctrl,secram_sec_r_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_socp_sec_w_ctrl,socp_sec_w_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_socp_sec_r_ctrl,socp_sec_r_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_others_sec_w_ctrl,others_sec_w_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_others_sec_r_ctrl,others_sec_r_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl1_ap_sec_ctrl,ap_sec_ctrl,HI_SEC_CTRL1_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL1_OFFSET)
HI_SET_GET(hi_sec_ctrl2_region1_filter_st_addr,region1_filter_st_addr,HI_SEC_CTRL2_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL2_OFFSET)
HI_SET_GET(hi_sec_ctrl3_region1_filter_end_addr,region1_filter_end_addr,HI_SEC_CTRL3_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL3_OFFSET)
HI_SET_GET(hi_sec_ctrl4_secram_filter_st_addr,secram_filter_st_addr,HI_SEC_CTRL4_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL4_OFFSET)
HI_SET_GET(hi_sec_ctrl5_secram_filter_end_addr,secram_filter_end_addr,HI_SEC_CTRL5_T,HI_SYSSC_BASE_ADDR, HI_SEC_CTRL5_OFFSET)
#endif

#endif

#endif // __HI_SYSSC_H__


