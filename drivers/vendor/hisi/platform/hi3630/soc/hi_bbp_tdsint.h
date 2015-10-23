/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_bbp_tdsint.h */
/* Version       : 2.0 */
/* Author        : xxx*/
/* Created       : 2013-04-09*/
/* Last Modified : */
/* Description   :  The C union definition file for the module bbp_tdsint*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Author        : xxx*/
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_BBP_TDSINT_H__
#define __HI_BBP_TDSINT_H__

/*
 * Project: hi
 * Module : bbp_tdsint
 */

#ifndef HI_SET_GET
#define HI_SET_GET(a0,a1,a2,a3,a4)
#endif

/********************************************************************************/
/*    bbp_tdsint 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_BBP_TDSINT_PUB_INT_MSK_OFFSET                  (0xA0) /* 子帧中断屏蔽寄存器 */
#define    HI_BBP_TDSINT_DSP_INT_MSK_OFFSET                  (0xA4) /* DSP时隙中断屏蔽寄存器 */
#define    HI_BBP_TDSINT_PUB_INT_CLEAR_OFFSET                (0xA8) /* 子帧中断清除寄存器 */
#define    HI_BBP_TDSINT_DSP_INT_CLEAR_OFFSET                (0xAC) /* DSP时隙中断清除寄存器 */
#define    HI_BBP_TDSINT_PUB_INT_ALM_OFFSET                  (0xB0) /* 子帧中断告警寄存器 */
#define    HI_BBP_TDSINT_DSP_INT_ALM_OFFSET                  (0xB4) /* DSP时隙中断告警寄存器 */


#ifndef __ASSEMBLY__

/********************************************************************************/
/*    bbp_tdsint 寄存器定义（项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    hifi_sfrm_int_msk          : 1; /* [0..0] hifi子帧中断屏蔽：1'b1时，STU模块产生子帧中断，并上报配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    arm_sfrm_int_msk           : 1; /* [1..1] arm子帧中断屏蔽：1'b1时，STU模块产生子帧中断，并上报配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSINT_PUB_INT_MSK_T;    /* 子帧中断屏蔽寄存器 */

typedef union
{
    struct
    {
        unsigned int    slot_int_msk               : 23; /* [22..0] DSP时隙中断屏蔽：1'b1时，打开时隙中断：bit22:bit21，分别为特殊时隙的2/3和1/3中断bit20:bit0，普通时隙0~6的中断屏蔽，其中每连续3-bit为一组，共7组，bit位由低到高分别表示对用时隙的1/4、1/2、3/中断屏蔽位。配置时刻：任意时刻生效时刻：立即 */
        unsigned int    dsp_sfrm_int_msk           : 1; /* [23..23] DSP子帧中断屏蔽：1'b1时，STU模块产生子帧中断，并上报配置时刻：任意时刻生效时刻：立即生效 */
        unsigned int    reserved                   : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSINT_DSP_INT_MSK_T;    /* DSP时隙中断屏蔽寄存器 */

typedef union
{
    struct
    {
        unsigned int    hifi_sfrm_int_clear        : 1; /* [0..0] hifi子帧中断清除信号，1'b1，清除子帧中断：配置时刻：任意时刻生效时刻：立即 */
        unsigned int    arm_sfrm_int_clear         : 1; /* [1..1] arm子帧中断清除信号，1'b1，清除子帧中断：配置时刻：任意时刻生效时刻：立即 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSINT_PUB_INT_CLEAR_T;    /* 子帧中断清除寄存器 */

typedef union
{
    struct
    {
        unsigned int    slot_int_clear             : 23; /* [22..0] DSP时隙中断清除：1'b1时，清除时隙中断：bit22:bit21，分别为特殊时隙的2/3和1/3中断清除bit20:bit0，普通时隙0~6的中断清除，其中每连续3-bit为一组，共7组，bit位由低到高分别表示对用时隙的1/4、1/2、3/中断清除位。配置时刻：任意时刻生效时刻：立即 */
        unsigned int    dsp_sfrm_int_clear         : 1; /* [23..23] DSP子帧中断清除信号，1'b1，清除子帧中断：配置时刻：任意时刻生效时刻：立即 */
        unsigned int    reserved                   : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSINT_DSP_INT_CLEAR_T;    /* DSP时隙中断清除寄存器 */

typedef union
{
    struct
    {
        unsigned int    hifi_sfrm_int_alm          : 1; /* [0..0] hifi子帧中断告警寄存器。当新中断到来而原来的中断还未被清除时，产生告警信号，置该位为1'b1。向该位写入1'b1可清除告警配置时刻：任意时刻生效时刻：立即 */
        unsigned int    arm_sfrm_int_alm           : 1; /* [1..1] arm子帧中断告警寄存器。当新中断到来而原来的中断还未被清除时，产生告警信号，置该位为1'b1。向该位写入1'b1可清除告警配置时刻：任意时刻生效时刻：立即 */
        unsigned int    reserved                   : 30; /* [31..2] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSINT_PUB_INT_ALM_T;    /* 子帧中断告警寄存器 */

typedef union
{
    struct
    {
        unsigned int    slot_int_alm               : 23; /* [22..0] 时隙中断告警寄存器。当新中断到来而之前的中断还未被清除时，产生告警，置该位为1'b1。向该位写入1'b1可清除告警位。bit22:bit21，分别为特殊时隙的2/3和1/3中告警bit20:bit0，普通时隙0~6的中断告警，其中每连续3-bit为一组，共7组，bit位由低到高分别表示对用时隙的1/4、1/2、3/中断告警位。配置时刻：任意时刻生效时刻：立即 */
        unsigned int    dsp_sfrm_int_alm           : 1; /* [23..23] DSP子帧中断告警寄存器。当新中断到来而原来的中断还未被清除时，产生告警信号，置该位为1'b1。向该位写入1'b1可清除告警配置时刻：任意时刻生效时刻：立即 */
        unsigned int    reserved                   : 8; /* [31..24] 保留 */
    } bits;
    unsigned int    u32;
}HI_BBP_TDSINT_DSP_INT_ALM_T;    /* DSP时隙中断告警寄存器 */


/********************************************************************************/
/*    bbp_tdsint 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
HI_SET_GET(hi_bbp_tdsint_pub_int_msk_hifi_sfrm_int_msk,hifi_sfrm_int_msk,HI_BBP_TDSINT_PUB_INT_MSK_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_PUB_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tdsint_pub_int_msk_arm_sfrm_int_msk,arm_sfrm_int_msk,HI_BBP_TDSINT_PUB_INT_MSK_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_PUB_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tdsint_pub_int_msk_reserved,reserved,HI_BBP_TDSINT_PUB_INT_MSK_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_PUB_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tdsint_dsp_int_msk_slot_int_msk,slot_int_msk,HI_BBP_TDSINT_DSP_INT_MSK_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_DSP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tdsint_dsp_int_msk_dsp_sfrm_int_msk,dsp_sfrm_int_msk,HI_BBP_TDSINT_DSP_INT_MSK_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_DSP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tdsint_dsp_int_msk_reserved,reserved,HI_BBP_TDSINT_DSP_INT_MSK_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_DSP_INT_MSK_OFFSET)
HI_SET_GET(hi_bbp_tdsint_pub_int_clear_hifi_sfrm_int_clear,hifi_sfrm_int_clear,HI_BBP_TDSINT_PUB_INT_CLEAR_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_PUB_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tdsint_pub_int_clear_arm_sfrm_int_clear,arm_sfrm_int_clear,HI_BBP_TDSINT_PUB_INT_CLEAR_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_PUB_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tdsint_pub_int_clear_reserved,reserved,HI_BBP_TDSINT_PUB_INT_CLEAR_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_PUB_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tdsint_dsp_int_clear_slot_int_clear,slot_int_clear,HI_BBP_TDSINT_DSP_INT_CLEAR_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_DSP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tdsint_dsp_int_clear_dsp_sfrm_int_clear,dsp_sfrm_int_clear,HI_BBP_TDSINT_DSP_INT_CLEAR_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_DSP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tdsint_dsp_int_clear_reserved,reserved,HI_BBP_TDSINT_DSP_INT_CLEAR_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_DSP_INT_CLEAR_OFFSET)
HI_SET_GET(hi_bbp_tdsint_pub_int_alm_hifi_sfrm_int_alm,hifi_sfrm_int_alm,HI_BBP_TDSINT_PUB_INT_ALM_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_PUB_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tdsint_pub_int_alm_arm_sfrm_int_alm,arm_sfrm_int_alm,HI_BBP_TDSINT_PUB_INT_ALM_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_PUB_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tdsint_pub_int_alm_reserved,reserved,HI_BBP_TDSINT_PUB_INT_ALM_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_PUB_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tdsint_dsp_int_alm_slot_int_alm,slot_int_alm,HI_BBP_TDSINT_DSP_INT_ALM_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_DSP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tdsint_dsp_int_alm_dsp_sfrm_int_alm,dsp_sfrm_int_alm,HI_BBP_TDSINT_DSP_INT_ALM_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_DSP_INT_ALM_OFFSET)
HI_SET_GET(hi_bbp_tdsint_dsp_int_alm_reserved,reserved,HI_BBP_TDSINT_DSP_INT_ALM_T,HI_BBP_TDSINT_BASE_ADDR, HI_BBP_TDSINT_DSP_INT_ALM_OFFSET)

#endif

#endif // __HI_BBP_TDSINT_H__

