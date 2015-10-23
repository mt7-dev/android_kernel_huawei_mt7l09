/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_spi.h */
/* Version       : 2.0 */
/* Created       : 2013-02-21*/
/* Last Modified : */
/* Description   :  The C union definition file for the module spi*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_SPI_H__
#define __HI_SPI_H__

/*
 * Project: hi
 * Module : spi
 */

#ifndef HI_SET_GET
#define HI_SET_GET(a0,a1,a2,a3,a4)
#endif

/********************************************************************************/
/*    spi 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_SPI_CTRLR0_OFFSET                              (0x0) /* 控制寄存器0。 */
#define    HI_SPI_CTRLR1_OFFSET                              (0x4) /* 控制寄存器1。 */
#define    HI_SPI_SSIENR_OFFSET                              (0x8) /* SSI使能寄存器。 */
#define    HI_SPI_MWCR_OFFSET                                (0xC) /* Microwire控制寄存器。 */
#define    HI_SPI_SER_OFFSET                                 (0x10) /* slave使能寄存器。 */
#define    HI_SPI_BAUDR_OFFSET                               (0x14) /* 波特率选择。 */
#define    HI_SPI_TXFTLR_OFFSET                              (0x18) /* 发送FIFO阈值。 */
#define    HI_SPI_RXFTLR_OFFSET                              (0x1C) /* 接收FIFO阈值。 */
#define    HI_SPI_TXFLR_OFFSET                               (0x20) /* 发送FIFO水线寄存器。 */
#define    HI_SPI_RXFLR_OFFSET                               (0x24) /* 接收FIFO水线寄存器。 */
#define    HI_SPI_SR_OFFSET                                  (0x28) /* 状态寄存器。 */
#define    HI_SPI_IMR_OFFSET                                 (0x2C) /* 中断屏蔽寄存器。 */
#define    HI_SPI_ISR_OFFSET                                 (0x30) /* 中断状态寄存器。 */
#define    HI_SPI_RISR_OFFSET                                (0x34) /* 原始中断寄存器。 */
#define    HI_SPI_TXOICR_OFFSET                              (0x38) /* 发送FIFO溢出中断清除寄存器。 */
#define    HI_SPI_RXOICR_OFFSET                              (0x3C) /* 接收FIFO溢出中断清除寄存器。 */
#define    HI_SPI_RXUICR_OFFSET                              (0x40) /* 接收FIFO下溢中断清除寄存器。 */
#define    HI_SPI_MSTICR_OFFSET                              (0x44) /* 多master冲突中断清除寄存器。 */
#define    HI_SPI_ICR_OFFSET                                 (0x48) /* 中断清除寄存器。 */
#define    HI_SPI_DMACR_OFFSET                               (0x4C) /* DMA控制寄存器。 */
#define    HI_SPI_DMATDLR_OFFSET                             (0x50) /* DMA发送数据水线。 */
#define    HI_SPI_DMARDLR_OFFSET                             (0x54) /* DMA接收数据水线。 */
#define    HI_SPI_IDR_OFFSET                                 (0x58) /* ID寄存器。 */
#define    HI_SPI_SSI_COMP_VERSION_OFFSET                    (0x5C) /* 版本寄存器。 */
#define    HI_SPI_DR_OFFSET                                  (0x60) /* 数据寄存器。 */
#define    HI_SPI_RX_SAMPLE_DLY_OFFSET                       (0xF0) /* 采样时间延迟寄存器 */

/********************************************************************************/
/*    spi 寄存器定义（项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    DFS                        : 4; /* [3..0] 数据帧大小。配置值必须大于等于3，否则配置值无法更新 */
        unsigned int    FRF                        : 2; /* [5..4] 帧格式。00－Motorola SPI01－Texas Instruments SSP10－National Semiconductors Microwire11－Reserved */
        unsigned int    SCPH                       : 1; /* [6..6] 串行时钟相位选择。0－串行时钟在第一个数据位中触发。1－串行时钟在第一个数据位开始触发。当SSI_HC_FRF=1和SSI_DFLT_FRF=0时，SCPH只读。 */
        unsigned int    SCPOL                      : 1; /* [7..7] 串行时钟极性选择。0－串行时钟低电平无效。1－串行时钟高电平无效。当SSI_HC_FRF=1和SSI_DFLT_FRF=0时，SCPOL只读。 */
        unsigned int    TMOD                       : 2; /* [9..8] 传输模式，选择传输模式。00－接受&发送01－发送10－接受11－EEPROM读 */
        unsigned int    SLV_OE                     : 1; /* [10..10] SLAVE输出使能，仅对应DW_apb_ssi配置为串行slave设备，当配置为串行master无效。0－使能Slave txd1－不使能Slave txd */
        unsigned int    SRL                        : 1; /* [11..11] 移位寄存器循环使能，测试用，内部连接发送移位寄存器输出和接受移位寄存器输入。 */
        unsigned int    CFS                        : 4; /* [15..12] 控制帧大小，选择Microwire帧格式控制字长度0~2:保留3~15:n+1位串行数据传输，n等于CFS */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_CTRLR0_T;    /* 控制寄存器0。 */

typedef union
{
    struct
    {
        unsigned int    NDF                        : 16; /* [15..0] 接收数据帧数目，为ndf+1 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_CTRLR1_T;    /* 控制寄存器1。 */

typedef union
{
    struct
    {
        unsigned int    SSI_EN                     : 1; /* [0..0] SSI使能。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SPI_SSIENR_T;    /* SSI使能寄存器。 */

typedef union
{
    struct
    {
        unsigned int    MWMOD                      : 1; /* [0..0] Microwire传输模式。0－非序列传输。1－序列传输。 */
        unsigned int    MDD                        : 1; /* [1..1] Microwire控制。当使用Microwire串行协议时定义数据方向。0－接收模式。1－发送模式。 */
        unsigned int    MHS                        : 1; /* [2..2] Microwire握手。仅对应DW_apb_ssi配置成串行master设备时。0－握手接口不使能。1－握手接口使能。 */
        unsigned int    reserved_1                 : 1; /* [3..3] 保留。 */
        unsigned int    mw_toggle_cyc              : 4; /* [7..4] 写到读的切换周期。0：0个切换周期。1：1个切换周期。2：2个切换周期。…… */
        unsigned int    reserved_0                 : 24; /* [31..8] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SPI_MWCR_T;    /* Microwire控制寄存器。 */

typedef union
{
    struct
    {
        unsigned int    SER                        : 4; /* [3..0] Slave选择使能标志，每一位表示一个Slave。0－没有选择1－选择 */
        unsigned int    reserved                   : 28; /* [31..4] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SPI_SER_T;    /* slave使能寄存器。 */

typedef union
{
    struct
    {
        unsigned int    SCKDV                      : 16; /* [15..0] SSI时钟分频比，不支持奇数分频。第0位自动屏蔽，以保证分频比为偶数。例如：配置SCKDV=3（2'b11）,实际分频比为2(2'b10) */
        unsigned int    reserved                   : 16; /* [31..16] 保留。 */
    } bits;
    unsigned int    u32;
}HI_SPI_BAUDR_T;    /* 波特率选择。 */

typedef union
{
    struct
    {
        unsigned int    TFT                        : 8; /* [7..0] 发送FIFO门槛。当FIFO内数据数目超过TFT，触发中断。 */
        unsigned int    reserved                   : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_TXFTLR_T;    /* 发送FIFO阈值。 */

typedef union
{
    struct
    {
        unsigned int    RFT                        : 3; /* [2..0] 接收FIFO门槛。当FIFO内数据数目超过RFT，触发中断。 */
        unsigned int    reserved_1                 : 5; /* [7..3] 保留。该保留比特段禁止写入非0值。 */
        unsigned int    reserved_0                 : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_RXFTLR_T;    /* 接收FIFO阈值。 */

typedef union
{
    struct
    {
        unsigned int    TXTFL                      : 9; /* [8..0] 发送FIFO内数据数目。 */
        unsigned int    reserved                   : 23; /* [31..9] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_TXFLR_T;    /* 发送FIFO水线寄存器。 */

typedef union
{
    struct
    {
        unsigned int    RXTFL                      : 4; /* [3..0] 接收FIFO内数据数目。 */
        unsigned int    reserved                   : 28; /* [31..4] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_RXFLR_T;    /* 接收FIFO水线寄存器。 */

typedef union
{
    struct
    {
        unsigned int    BUSY                       : 1; /* [0..0] SSI忙标志。0－DW_apb_ssi空闲或未使能1－DW_apb_ssi正在搬运数据 */
        unsigned int    TFNF                       : 1; /* [1..1] 发送FIFO未满。0－发送FIFO满1－发送FIFO未满 */
        unsigned int    TFE                        : 1; /* [2..2] 发送FIFO空。0－发送FIFO非空1－发送FIFO空 */
        unsigned int    RFNE                       : 1; /* [3..3] 接收FIFO非空。0－接收FIFO空1－接收FIFO非空 */
        unsigned int    RFF                        : 1; /* [4..4] 接收FIFO满。当接收FIFO内数据全满时置1，否则清0。0－接收FIFO未满1－接收FIFO满 */
        unsigned int    TXE                        : 1; /* [5..5] 发送错误。当传输启动后发送FIFO内无数据TXE置1。0－无错1－发送出错仅slave模式时使用 */
        unsigned int    DCOL                       : 1; /* [6..6] 数据冲突错误。仅对应DW_apb_ssi配置为master时，其它master选择此设备作为slave。0－无错误。1－发送数据冲突出错 */
        unsigned int    reserved                   : 25; /* [31..7] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_SR_T;    /* 状态寄存器。 */

typedef union
{
    struct
    {
        unsigned int    TXEIM                      : 1; /* [0..0] 发送FIFO空中断屏蔽。0－ssi_txe_intr中断被屏蔽1－ssi_txe_intr中断没有屏蔽 */
        unsigned int    TXOIM                      : 1; /* [1..1] 发送FIFO溢出中断屏蔽0－ssi_txo_intr中断被屏蔽1－ssi_txo_intr中断没有屏蔽 */
        unsigned int    RXUIM                      : 1; /* [2..2] 接收FIFO下溢出中断屏蔽。0－ssi_rxu_intr中断被屏蔽1－ssi_rxu_intr中断没有屏蔽 */
        unsigned int    RXOIM                      : 1; /* [3..3] 接收FIFO溢出中断屏蔽。0－ssi_rxo_intr中断被屏蔽1－ssi_rxo_intr中断没有屏蔽 */
        unsigned int    RXFIM                      : 1; /* [4..4] 接收FIFO满中断屏蔽。0－ssi_rxf_intr中断被屏蔽1－ssi_rxf_intr中断没有屏蔽 */
        unsigned int    MSTIM                      : 1; /* [5..5] 多master竞争中断屏蔽。0－ssi_mst_intr中断被屏蔽1－ssi_mst_intr中断未屏蔽 */
        unsigned int    reserved                   : 26; /* [31..6] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_IMR_T;    /* 中断屏蔽寄存器。 */

typedef union
{
    struct
    {
        unsigned int    TXEIS                      : 1; /* [0..0] 发送FIFO空中断状态。0－ssi_txe_intr中断屏蔽后没有激活1－ssi_txe_intr中断屏蔽后激活 */
        unsigned int    TXOIS                      : 1; /* [1..1] 发送FIFO溢出中断状态0－ssi_txo_intr中断屏蔽后没有激活1－ssi_txo_intr中断屏蔽后激活 */
        unsigned int    RXUIS                      : 1; /* [2..2] 接收FIFO下溢出中断状态。0－ssi_rxo_intr中断屏蔽后没有激活1－ssi_rxo_intr中断屏蔽后激活 */
        unsigned int    RXOIS                      : 1; /* [3..3] 接收FIFO上溢出中断状态。0－ssi_rxu_intr中断屏蔽后没有激活1－ssi_rxu_intr中断屏蔽后激活 */
        unsigned int    RXFIS                      : 1; /* [4..4] 接收FIFO满中断状态。0－ssi_rxf_intr中断屏蔽后没有激活1－ssi_rxf_intr中断屏蔽后激活 */
        unsigned int    MSTIS                      : 1; /* [5..5] 多master竞争中断状态。0－ssi_mst_intr中断屏蔽后没有激活1－ssi_mst_intr中断屏蔽后激活 */
        unsigned int    reserved                   : 26; /* [31..6] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_ISR_T;    /* 中断状态寄存器。 */

typedef union
{
    struct
    {
        unsigned int    TXEIR                      : 1; /* [0..0] 发送FIFO空裸中断状态。0－ssi_txe_intr中断屏蔽前没有激活1－ssi_txe_intr中断屏蔽前激活 */
        unsigned int    TXOIR                      : 1; /* [1..1] 发送FIFO溢出裸中断状态0－ssi_txo_intr中断屏蔽前没有激活1－ssi_txo_intr中断屏蔽前激活 */
        unsigned int    RXUIR                      : 1; /* [2..2] 接收FIFO下溢出裸中断状态。0－ssi_rxu_intr中断屏蔽前没有激活1－ssi_rxu_intr中断屏蔽前激活 */
        unsigned int    RXOIR                      : 1; /* [3..3] 接收FIFO溢出裸中断状态。0－ssi_rxo_intr中断屏蔽前没有激活1－ssi_rxo_intr中断屏蔽前激活 */
        unsigned int    RXFIR                      : 1; /* [4..4] 接收FIFO满裸中断状态。0－ssi_mst_intr中断屏蔽前没有激活1－ssi_mst_intr中断屏蔽前激活 */
        unsigned int    MSTIR                      : 1; /* [5..5] 多master竞争裸中断状态。0－ssi_mst_intr中断屏蔽前没有激活1－ssi_mst_intr中断屏蔽前激活 */
        unsigned int    reserved                   : 26; /* [31..6] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_RISR_T;    /* 原始中断寄存器。 */

typedef union
{
    struct
    {
        unsigned int    TXOICR                     : 1; /* [0..0] 读清0发送FIFO溢出中断ssi_txo_intr。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_TXOICR_T;    /* 发送FIFO溢出中断清除寄存器。 */

typedef union
{
    struct
    {
        unsigned int    RXOICR                     : 1; /* [0..0] 读清0接收FIFO溢出中断ssi_rxo_intr。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_RXOICR_T;    /* 接收FIFO溢出中断清除寄存器。 */

typedef union
{
    struct
    {
        unsigned int    RXUICR                     : 1; /* [0..0] 读清0接收FIFO下溢出中断ssi_rxu_intr。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_RXUICR_T;    /* 接收FIFO下溢中断清除寄存器。 */

typedef union
{
    struct
    {
        unsigned int    MSTICR                     : 1; /* [0..0] 读清0多master竞争中断ssi_mst_intr。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_MSTICR_T;    /* 多master冲突中断清除寄存器。 */

typedef union
{
    struct
    {
        unsigned int    ICR                        : 1; /* [0..0] 读清0中断ssi_txo_intr、ssi_rxu_intr、ssi_rxo_intr和ssi_mst_intr。 */
        unsigned int    reserved                   : 31; /* [31..1] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_ICR_T;    /* 中断清除寄存器。 */

typedef union
{
    struct
    {
        unsigned int    RDMAE                      : 1; /* [0..0] 接收DMA使能0－接收DMA不使能1－接收DMA使能 */
        unsigned int    TDMAE                      : 1; /* [1..1] 发送DMA使能0－发送DMA不使能1－发送DMA使能 */
        unsigned int    PMI0_IND                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_DMACR_T;    /* DMA控制寄存器。 */

typedef union
{
    struct
    {
        unsigned int    DMATDL                     : 3; /* [2..0] 发送数据水平线。当小于等于DMATDL个有效数据进入发送FIFO，并且TDMAE＝1，激活dma_tx_req请求 */
        unsigned int    reserved_1                 : 5; /* [7..3] 保留。该保留比特段禁止写入非0值。 */
        unsigned int    reserved_0                 : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_DMATDLR_T;    /* DMA发送数据水线。 */

typedef union
{
    struct
    {
        unsigned int    DMARDL                     : 3; /* [2..0] 接收数据水平线。当大于等于DMARDL＋1个有效数据进入接收FIFO，并且TDMAE＝1，激活dma_tx_req请求。 */
        unsigned int    reserved_1                 : 5; /* [7..3] 保留。该保留比特段禁止写入非0值。 */
        unsigned int    reserved_0                 : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_DMARDLR_T;    /* DMA接收数据水线。 */

typedef union
{
    struct
    {
        unsigned int    IDCODE                     : 32; /* [31..0] 识别码。 */
    } bits;
    unsigned int    u32;
}HI_SPI_IDR_T;    /* ID寄存器。 */

typedef union
{
    struct
    {
        unsigned int    SSI_COMP_VERSION           : 32; /* [31..0] synopsys器件版本。表示4位ASCII码。 */
    } bits;
    unsigned int    u32;
}HI_SPI_SSI_COMP_VERSION_T;    /* 版本寄存器。 */

typedef union
{
    struct
    {
        unsigned int    DR                         : 16; /* [15..0] 数据寄存器。写时必须右调整，读时自动调整。写时DR为发送FIFO缓存。读时DR为接收FIFO缓存。 */
        unsigned int    reserved                   : 16; /* [31..16] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_DR_T;    /* 数据寄存器。 */

typedef union
{
    struct
    {
        unsigned int    RSD                        : 8; /* [7..0] master回读采样延迟周期配置0：不延迟1：回读采样延迟1个ssi_clk2：回读采样延迟2个ssi_clk…… */
        unsigned int    reserved                   : 24; /* [31..8] 保留 */
    } bits;
    unsigned int    u32;
}HI_SPI_RX_SAMPLE_DLY_T;    /* 采样时间延迟寄存器 */


/********************************************************************************/
/*    spi 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
HI_SET_GET(hi_spi_ctrlr0_dfs,dfs,HI_SPI_CTRLR0_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR0_OFFSET)
HI_SET_GET(hi_spi_ctrlr0_frf,frf,HI_SPI_CTRLR0_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR0_OFFSET)
HI_SET_GET(hi_spi_ctrlr0_scph,scph,HI_SPI_CTRLR0_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR0_OFFSET)
HI_SET_GET(hi_spi_ctrlr0_scpol,scpol,HI_SPI_CTRLR0_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR0_OFFSET)
HI_SET_GET(hi_spi_ctrlr0_tmod,tmod,HI_SPI_CTRLR0_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR0_OFFSET)
HI_SET_GET(hi_spi_ctrlr0_slv_oe,slv_oe,HI_SPI_CTRLR0_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR0_OFFSET)
HI_SET_GET(hi_spi_ctrlr0_srl,srl,HI_SPI_CTRLR0_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR0_OFFSET)
HI_SET_GET(hi_spi_ctrlr0_cfs,cfs,HI_SPI_CTRLR0_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR0_OFFSET)
HI_SET_GET(hi_spi_ctrlr0_reserved,reserved,HI_SPI_CTRLR0_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR0_OFFSET)
HI_SET_GET(hi_spi_ctrlr1_ndf,ndf,HI_SPI_CTRLR1_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR1_OFFSET)
HI_SET_GET(hi_spi_ctrlr1_reserved,reserved,HI_SPI_CTRLR1_T,HI_SPI_BASE_ADDR, HI_SPI_CTRLR1_OFFSET)
HI_SET_GET(hi_spi_ssienr_ssi_en,ssi_en,HI_SPI_SSIENR_T,HI_SPI_BASE_ADDR, HI_SPI_SSIENR_OFFSET)
HI_SET_GET(hi_spi_ssienr_reserved,reserved,HI_SPI_SSIENR_T,HI_SPI_BASE_ADDR, HI_SPI_SSIENR_OFFSET)
HI_SET_GET(hi_spi_mwcr_mwmod,mwmod,HI_SPI_MWCR_T,HI_SPI_BASE_ADDR, HI_SPI_MWCR_OFFSET)
HI_SET_GET(hi_spi_mwcr_mdd,mdd,HI_SPI_MWCR_T,HI_SPI_BASE_ADDR, HI_SPI_MWCR_OFFSET)
HI_SET_GET(hi_spi_mwcr_mhs,mhs,HI_SPI_MWCR_T,HI_SPI_BASE_ADDR, HI_SPI_MWCR_OFFSET)
HI_SET_GET(hi_spi_mwcr_reserved_1,reserved_1,HI_SPI_MWCR_T,HI_SPI_BASE_ADDR, HI_SPI_MWCR_OFFSET)
HI_SET_GET(hi_spi_mwcr_mw_toggle_cyc,mw_toggle_cyc,HI_SPI_MWCR_T,HI_SPI_BASE_ADDR, HI_SPI_MWCR_OFFSET)
HI_SET_GET(hi_spi_mwcr_reserved_0,reserved_0,HI_SPI_MWCR_T,HI_SPI_BASE_ADDR, HI_SPI_MWCR_OFFSET)
HI_SET_GET(hi_spi_ser_ser,ser,HI_SPI_SER_T,HI_SPI_BASE_ADDR, HI_SPI_SER_OFFSET)
HI_SET_GET(hi_spi_ser_reserved,reserved,HI_SPI_SER_T,HI_SPI_BASE_ADDR, HI_SPI_SER_OFFSET)
HI_SET_GET(hi_spi_baudr_sckdv,sckdv,HI_SPI_BAUDR_T,HI_SPI_BASE_ADDR, HI_SPI_BAUDR_OFFSET)
HI_SET_GET(hi_spi_baudr_reserved,reserved,HI_SPI_BAUDR_T,HI_SPI_BASE_ADDR, HI_SPI_BAUDR_OFFSET)
HI_SET_GET(hi_spi_txftlr_tft,tft,HI_SPI_TXFTLR_T,HI_SPI_BASE_ADDR, HI_SPI_TXFTLR_OFFSET)
HI_SET_GET(hi_spi_txftlr_reserved,reserved,HI_SPI_TXFTLR_T,HI_SPI_BASE_ADDR, HI_SPI_TXFTLR_OFFSET)
HI_SET_GET(hi_spi_rxftlr_rft,rft,HI_SPI_RXFTLR_T,HI_SPI_BASE_ADDR, HI_SPI_RXFTLR_OFFSET)
HI_SET_GET(hi_spi_rxftlr_reserved_1,reserved_1,HI_SPI_RXFTLR_T,HI_SPI_BASE_ADDR, HI_SPI_RXFTLR_OFFSET)
HI_SET_GET(hi_spi_rxftlr_reserved_0,reserved_0,HI_SPI_RXFTLR_T,HI_SPI_BASE_ADDR, HI_SPI_RXFTLR_OFFSET)
HI_SET_GET(hi_spi_txflr_txtfl,txtfl,HI_SPI_TXFLR_T,HI_SPI_BASE_ADDR, HI_SPI_TXFLR_OFFSET)
HI_SET_GET(hi_spi_txflr_reserved,reserved,HI_SPI_TXFLR_T,HI_SPI_BASE_ADDR, HI_SPI_TXFLR_OFFSET)
HI_SET_GET(hi_spi_rxflr_rxtfl,rxtfl,HI_SPI_RXFLR_T,HI_SPI_BASE_ADDR, HI_SPI_RXFLR_OFFSET)
HI_SET_GET(hi_spi_rxflr_reserved,reserved,HI_SPI_RXFLR_T,HI_SPI_BASE_ADDR, HI_SPI_RXFLR_OFFSET)
HI_SET_GET(hi_spi_sr_busy,busy,HI_SPI_SR_T,HI_SPI_BASE_ADDR, HI_SPI_SR_OFFSET)
HI_SET_GET(hi_spi_sr_tfnf,tfnf,HI_SPI_SR_T,HI_SPI_BASE_ADDR, HI_SPI_SR_OFFSET)
HI_SET_GET(hi_spi_sr_tfe,tfe,HI_SPI_SR_T,HI_SPI_BASE_ADDR, HI_SPI_SR_OFFSET)
HI_SET_GET(hi_spi_sr_rfne,rfne,HI_SPI_SR_T,HI_SPI_BASE_ADDR, HI_SPI_SR_OFFSET)
HI_SET_GET(hi_spi_sr_rff,rff,HI_SPI_SR_T,HI_SPI_BASE_ADDR, HI_SPI_SR_OFFSET)
HI_SET_GET(hi_spi_sr_txe,txe,HI_SPI_SR_T,HI_SPI_BASE_ADDR, HI_SPI_SR_OFFSET)
HI_SET_GET(hi_spi_sr_dcol,dcol,HI_SPI_SR_T,HI_SPI_BASE_ADDR, HI_SPI_SR_OFFSET)
HI_SET_GET(hi_spi_sr_reserved,reserved,HI_SPI_SR_T,HI_SPI_BASE_ADDR, HI_SPI_SR_OFFSET)
HI_SET_GET(hi_spi_imr_txeim,txeim,HI_SPI_IMR_T,HI_SPI_BASE_ADDR, HI_SPI_IMR_OFFSET)
HI_SET_GET(hi_spi_imr_txoim,txoim,HI_SPI_IMR_T,HI_SPI_BASE_ADDR, HI_SPI_IMR_OFFSET)
HI_SET_GET(hi_spi_imr_rxuim,rxuim,HI_SPI_IMR_T,HI_SPI_BASE_ADDR, HI_SPI_IMR_OFFSET)
HI_SET_GET(hi_spi_imr_rxoim,rxoim,HI_SPI_IMR_T,HI_SPI_BASE_ADDR, HI_SPI_IMR_OFFSET)
HI_SET_GET(hi_spi_imr_rxfim,rxfim,HI_SPI_IMR_T,HI_SPI_BASE_ADDR, HI_SPI_IMR_OFFSET)
HI_SET_GET(hi_spi_imr_mstim,mstim,HI_SPI_IMR_T,HI_SPI_BASE_ADDR, HI_SPI_IMR_OFFSET)
HI_SET_GET(hi_spi_imr_reserved,reserved,HI_SPI_IMR_T,HI_SPI_BASE_ADDR, HI_SPI_IMR_OFFSET)
HI_SET_GET(hi_spi_isr_txeis,txeis,HI_SPI_ISR_T,HI_SPI_BASE_ADDR, HI_SPI_ISR_OFFSET)
HI_SET_GET(hi_spi_isr_txois,txois,HI_SPI_ISR_T,HI_SPI_BASE_ADDR, HI_SPI_ISR_OFFSET)
HI_SET_GET(hi_spi_isr_rxuis,rxuis,HI_SPI_ISR_T,HI_SPI_BASE_ADDR, HI_SPI_ISR_OFFSET)
HI_SET_GET(hi_spi_isr_rxois,rxois,HI_SPI_ISR_T,HI_SPI_BASE_ADDR, HI_SPI_ISR_OFFSET)
HI_SET_GET(hi_spi_isr_rxfis,rxfis,HI_SPI_ISR_T,HI_SPI_BASE_ADDR, HI_SPI_ISR_OFFSET)
HI_SET_GET(hi_spi_isr_mstis,mstis,HI_SPI_ISR_T,HI_SPI_BASE_ADDR, HI_SPI_ISR_OFFSET)
HI_SET_GET(hi_spi_isr_reserved,reserved,HI_SPI_ISR_T,HI_SPI_BASE_ADDR, HI_SPI_ISR_OFFSET)
HI_SET_GET(hi_spi_risr_txeir,txeir,HI_SPI_RISR_T,HI_SPI_BASE_ADDR, HI_SPI_RISR_OFFSET)
HI_SET_GET(hi_spi_risr_txoir,txoir,HI_SPI_RISR_T,HI_SPI_BASE_ADDR, HI_SPI_RISR_OFFSET)
HI_SET_GET(hi_spi_risr_rxuir,rxuir,HI_SPI_RISR_T,HI_SPI_BASE_ADDR, HI_SPI_RISR_OFFSET)
HI_SET_GET(hi_spi_risr_rxoir,rxoir,HI_SPI_RISR_T,HI_SPI_BASE_ADDR, HI_SPI_RISR_OFFSET)
HI_SET_GET(hi_spi_risr_rxfir,rxfir,HI_SPI_RISR_T,HI_SPI_BASE_ADDR, HI_SPI_RISR_OFFSET)
HI_SET_GET(hi_spi_risr_mstir,mstir,HI_SPI_RISR_T,HI_SPI_BASE_ADDR, HI_SPI_RISR_OFFSET)
HI_SET_GET(hi_spi_risr_reserved,reserved,HI_SPI_RISR_T,HI_SPI_BASE_ADDR, HI_SPI_RISR_OFFSET)
HI_SET_GET(hi_spi_txoicr_txoicr,txoicr,HI_SPI_TXOICR_T,HI_SPI_BASE_ADDR, HI_SPI_TXOICR_OFFSET)
HI_SET_GET(hi_spi_txoicr_reserved,reserved,HI_SPI_TXOICR_T,HI_SPI_BASE_ADDR, HI_SPI_TXOICR_OFFSET)
HI_SET_GET(hi_spi_rxoicr_rxoicr,rxoicr,HI_SPI_RXOICR_T,HI_SPI_BASE_ADDR, HI_SPI_RXOICR_OFFSET)
HI_SET_GET(hi_spi_rxoicr_reserved,reserved,HI_SPI_RXOICR_T,HI_SPI_BASE_ADDR, HI_SPI_RXOICR_OFFSET)
HI_SET_GET(hi_spi_rxuicr_rxuicr,rxuicr,HI_SPI_RXUICR_T,HI_SPI_BASE_ADDR, HI_SPI_RXUICR_OFFSET)
HI_SET_GET(hi_spi_rxuicr_reserved,reserved,HI_SPI_RXUICR_T,HI_SPI_BASE_ADDR, HI_SPI_RXUICR_OFFSET)
HI_SET_GET(hi_spi_msticr_msticr,msticr,HI_SPI_MSTICR_T,HI_SPI_BASE_ADDR, HI_SPI_MSTICR_OFFSET)
HI_SET_GET(hi_spi_msticr_reserved,reserved,HI_SPI_MSTICR_T,HI_SPI_BASE_ADDR, HI_SPI_MSTICR_OFFSET)
HI_SET_GET(hi_spi_icr_icr,icr,HI_SPI_ICR_T,HI_SPI_BASE_ADDR, HI_SPI_ICR_OFFSET)
HI_SET_GET(hi_spi_icr_reserved,reserved,HI_SPI_ICR_T,HI_SPI_BASE_ADDR, HI_SPI_ICR_OFFSET)
HI_SET_GET(hi_spi_dmacr_rdmae,rdmae,HI_SPI_DMACR_T,HI_SPI_BASE_ADDR, HI_SPI_DMACR_OFFSET)
HI_SET_GET(hi_spi_dmacr_tdmae,tdmae,HI_SPI_DMACR_T,HI_SPI_BASE_ADDR, HI_SPI_DMACR_OFFSET)
HI_SET_GET(hi_spi_dmacr_pmi0_ind,pmi0_ind,HI_SPI_DMACR_T,HI_SPI_BASE_ADDR, HI_SPI_DMACR_OFFSET)
HI_SET_GET(hi_spi_dmatdlr_dmatdl,dmatdl,HI_SPI_DMATDLR_T,HI_SPI_BASE_ADDR, HI_SPI_DMATDLR_OFFSET)
HI_SET_GET(hi_spi_dmatdlr_reserved_1,reserved_1,HI_SPI_DMATDLR_T,HI_SPI_BASE_ADDR, HI_SPI_DMATDLR_OFFSET)
HI_SET_GET(hi_spi_dmatdlr_reserved_0,reserved_0,HI_SPI_DMATDLR_T,HI_SPI_BASE_ADDR, HI_SPI_DMATDLR_OFFSET)
HI_SET_GET(hi_spi_dmardlr_dmardl,dmardl,HI_SPI_DMARDLR_T,HI_SPI_BASE_ADDR, HI_SPI_DMARDLR_OFFSET)
HI_SET_GET(hi_spi_dmardlr_reserved_1,reserved_1,HI_SPI_DMARDLR_T,HI_SPI_BASE_ADDR, HI_SPI_DMARDLR_OFFSET)
HI_SET_GET(hi_spi_dmardlr_reserved_0,reserved_0,HI_SPI_DMARDLR_T,HI_SPI_BASE_ADDR, HI_SPI_DMARDLR_OFFSET)
HI_SET_GET(hi_spi_idr_idcode,idcode,HI_SPI_IDR_T,HI_SPI_BASE_ADDR, HI_SPI_IDR_OFFSET)
HI_SET_GET(hi_spi_ssi_comp_version_ssi_comp_version,ssi_comp_version,HI_SPI_SSI_COMP_VERSION_T,HI_SPI_BASE_ADDR, HI_SPI_SSI_COMP_VERSION_OFFSET)
HI_SET_GET(hi_spi_dr_dr,dr,HI_SPI_DR_T,HI_SPI_BASE_ADDR, HI_SPI_DR_OFFSET)
HI_SET_GET(hi_spi_dr_reserved,reserved,HI_SPI_DR_T,HI_SPI_BASE_ADDR, HI_SPI_DR_OFFSET)
HI_SET_GET(hi_spi_rx_sample_dly_rsd,rsd,HI_SPI_RX_SAMPLE_DLY_T,HI_SPI_BASE_ADDR, HI_SPI_RX_SAMPLE_DLY_OFFSET)
HI_SET_GET(hi_spi_rx_sample_dly_reserved,reserved,HI_SPI_RX_SAMPLE_DLY_T,HI_SPI_BASE_ADDR, HI_SPI_RX_SAMPLE_DLY_OFFSET)

#endif // __HI_SPI_H__

