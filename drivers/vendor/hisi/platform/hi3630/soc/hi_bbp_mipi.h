/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_bbp_mipi.h */
/* Version       : 2.0 */
/* Created       : 2013-03-19*/
/* Last Modified : */
/* Description   :  The C union definition file for the module bbp_mipi*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_BBP_MIPI_H__
#define __HI_BBP_MIPI_H__

/*
 * Project: hi
 * Module : bbp_mipi
 */

#ifndef HI_SET_GET_MIPI
#define HI_SET_GET_MIPI(a0,a1,a2,a3,a4)
#endif

/********************************************************************************/
/*    bbp_mipi 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    BBP_MIPI_MIPI0_RD_DATA_LOW_SOFT_OFFSET            (0xD8) /* 寄存器 */
#define    BBP_MIPI_MIPI0_RD_DATA_HIGH_SOFT_OFFSET           (0xDC) /* 寄存器 */
#define    BBP_MIPI_RD_END_FLAG_MIPI0_SOFT_OFFSET            (0xE0) /* 寄存器 */
#define    BBP_MIPI_MIPI0_GRANT_DSP_OFFSET                   (0xE4) /* 寄存器 */
#define    BBP_MIPI_DSP_MIPI0_WDATA_LOW_OFFSET               (0xE8) /* 寄存器 */
#define    BBP_MIPI_DSP_MIPI0_WDATA_HIGH_OFFSET              (0xEC) /* 寄存器 */
#define    BBP_MIPI_DSP_MIPI0_EN_IMI_OFFSET                  (0xF0) /* 寄存器 */
#define    BBP_MIPI_DSP_MIPI0_CFG_IND_IMI_OFFSET             (0xF4) /* 寄存器 */
#define    BBP_MIPI_DSP_MIPI0_RD_CLR_OFFSET                  (0xF8) /* 寄存器 */

/********************************************************************************/
/*    bbp_mipi 寄存器定义（项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    mipi0_rd_data_low_soft     : 32; /* [31..0] 由软件直接控制MIPI0 master,不借助xbbp发送回读指令时,回读指令存放低32bit. */
    } bits;
    unsigned int    u32;
}BBP_MIPI_MIPI0_RD_DATA_LOW_SOFT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi0_rd_data_high_soft    : 32; /* [31..0] 由软件直接控制MIPI0 master,不借助xbbp发送回读指令时,回读指令存放高32bit. */
    } bits;
    unsigned int    u32;
}BBP_MIPI_MIPI0_RD_DATA_HIGH_SOFT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    rd_end_flag_mipi0_soft     : 1; /* [0..0] 由软件直接控制MIPI0 master,不借助xbbp发送回读指令时,回读数据有效指示 */
        unsigned int    Reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}BBP_MIPI_RD_END_FLAG_MIPI0_SOFT_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    mipi0_grant_dsp            : 1; /* [0..0] 由软件直接控制MIPI0 master1:软件可以发送CFG_IND启动mipi00:软件等待mipi0授权使用 */
        unsigned int    Reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}BBP_MIPI_MIPI0_GRANT_DSP_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi0_wdata_low        : 32; /* [31..0] 由软件直接控制MIPI0 master,不借助xbbp发送写指令时,写指令低32bit */
    } bits;
    unsigned int    u32;
}BBP_MIPI_DSP_MIPI0_WDATA_LOW_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi0_wdata_high       : 32; /* [31..0] 由软件直接控制MIPI0 master,不借助xbbp发送写指令时,写指令高32bit */
    } bits;
    unsigned int    u32;
}BBP_MIPI_DSP_MIPI0_WDATA_HIGH_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi0_en_imi           : 1; /* [0..0] 由软件直接控制MIPI0 master,需首先拉高此指示,屏蔽xbbp对MIPI0的控制 */
        unsigned int    Reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}BBP_MIPI_DSP_MIPI0_EN_IMI_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi0_cfg_ind_imi      : 1; /* [0..0] 由软件直接控制MIPI0 master,拉高此dsp_mipi0_en后,产生软件启动脉冲 */
        unsigned int    Reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}BBP_MIPI_DSP_MIPI0_CFG_IND_IMI_T;    /* 寄存器 */

typedef union
{
    struct
    {
        unsigned int    dsp_mipi0_rd_clr           : 1; /* [0..0] 由软件直接控制MIPI0 master,取走回读数据后,清标志位脉冲 */
        unsigned int    Reserved                   : 31; /* [31..1]  */
    } bits;
    unsigned int    u32;
}BBP_MIPI_DSP_MIPI0_RD_CLR_T;    /* 寄存器 */

extern u32 BBP_MIPI_BASE_ADDR[MIPI_BUTT];

/********************************************************************************/
/*    bbp_mipi 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
HI_SET_GET_MIPI(bbp_mipi_mipi0_rd_data_low_soft_mipi0_rd_data_low_soft,mipi0_rd_data_low_soft,BBP_MIPI_MIPI0_RD_DATA_LOW_SOFT_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_MIPI0_RD_DATA_LOW_SOFT_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_mipi0_rd_data_high_soft_mipi0_rd_data_high_soft,mipi0_rd_data_high_soft,BBP_MIPI_MIPI0_RD_DATA_HIGH_SOFT_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_MIPI0_RD_DATA_HIGH_SOFT_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_rd_end_flag_mipi0_soft_rd_end_flag_mipi0_soft,rd_end_flag_mipi0_soft,BBP_MIPI_RD_END_FLAG_MIPI0_SOFT_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_RD_END_FLAG_MIPI0_SOFT_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_rd_end_flag_mipi0_soft_reserved,Reserved,BBP_MIPI_RD_END_FLAG_MIPI0_SOFT_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_RD_END_FLAG_MIPI0_SOFT_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_mipi0_grant_dsp_mipi0_grant_dsp,mipi0_grant_dsp,BBP_MIPI_MIPI0_GRANT_DSP_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_MIPI0_GRANT_DSP_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_mipi0_grant_dsp_reserved,Reserved,BBP_MIPI_MIPI0_GRANT_DSP_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_MIPI0_GRANT_DSP_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_dsp_mipi0_wdata_low_dsp_mipi0_wdata_low,dsp_mipi0_wdata_low,BBP_MIPI_DSP_MIPI0_WDATA_LOW_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_DSP_MIPI0_WDATA_LOW_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_dsp_mipi0_wdata_high_dsp_mipi0_wdata_high,dsp_mipi0_wdata_high,BBP_MIPI_DSP_MIPI0_WDATA_HIGH_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_DSP_MIPI0_WDATA_HIGH_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_dsp_mipi0_en_imi_dsp_mipi0_en_imi,dsp_mipi0_en_imi,BBP_MIPI_DSP_MIPI0_EN_IMI_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_DSP_MIPI0_EN_IMI_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_dsp_mipi0_en_imi_reserved,Reserved,BBP_MIPI_DSP_MIPI0_EN_IMI_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_DSP_MIPI0_EN_IMI_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_dsp_mipi0_cfg_ind_imi_dsp_mipi0_cfg_ind_imi,dsp_mipi0_cfg_ind_imi,BBP_MIPI_DSP_MIPI0_CFG_IND_IMI_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_DSP_MIPI0_CFG_IND_IMI_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_dsp_mipi0_cfg_ind_imi_reserved,Reserved,BBP_MIPI_DSP_MIPI0_CFG_IND_IMI_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_DSP_MIPI0_CFG_IND_IMI_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_dsp_mipi0_rd_clr_dsp_mipi0_rd_clr,dsp_mipi0_rd_clr,BBP_MIPI_DSP_MIPI0_RD_CLR_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_DSP_MIPI0_RD_CLR_OFFSET)
HI_SET_GET_MIPI(bbp_mipi_dsp_mipi0_rd_clr_reserved,Reserved,BBP_MIPI_DSP_MIPI0_RD_CLR_T,BBP_MIPI_BASE_ADDR, BBP_MIPI_DSP_MIPI0_RD_CLR_OFFSET)

#endif // __HI_BBP_MIPI_H__

