/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_gpio.h */
/* Version       : 2.0 */
/* Created       : 2013-02-18*/
/* Last Modified : */
/* Description   :  The C union definition file for the module gpio*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_GPIO_H__
#define __HI_GPIO_H__



#define HI_K3_GPIO

/********************************************************************************/
/*    gpio 寄存器偏移定义（项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_GPIO_SWPORT_DR_OFFSET                          (0x0) /* GPIO输出数据寄存器。 */
#define    HI_GPIO_SWPORT_DDR_OFFSET                         (0x4) /* GPIO数据方向控制寄存器。 */
#define    HI_GPIO_INTEN_OFFSET                              (0x30) /* GPIO中断使能寄存器。 */
#define    HI_GPIO_INTMASK_OFFSET                            (0x34) /* GPIO中断屏蔽寄存器。 */
#define    HI_GPIO_INTTYPE_LEVEL_OFFSET                      (0x38) /* GPIO中断触发类型寄存器。 */
#define    HI_GPIO_INT_PLOARITY_OFFSET                       (0x3C) /* GPIO中断极性寄存器。 */
#define    HI_GPIO_INTSTATUS_OFFSET                          (0x40) /* GPIO中断状态寄存器。 */
#define    HI_GPIO_RAWINTSTATUS_OFFSET                       (0x44) /* GPIO原始中断状态寄存器。 */
#define    HI_GPIO_PORT_EOI_OFFSET                           (0x4C) /* GPIO中断清除寄存器。 */
#define    HI_GPIO_EXT_PORT_OFFSET                           (0x50) /* GPIO输入数据寄存器。 */

#endif // __HI_GPIO_H__

