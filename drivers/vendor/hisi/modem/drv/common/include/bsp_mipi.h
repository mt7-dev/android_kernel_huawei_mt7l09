/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : bsp_mipi.h */
/* Version       : 2.0 */
/* Created       : 2013-03-13*/
/* Last Modified : */
/* Description   :  The C union definition file for the module apb_mipi*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
#include "osl_types.h"
#include <bsp_om.h>

#ifndef MIPI_OK
#define MIPI_OK 0
#endif
#ifndef MIPI_ERROR
#define MIPI_ERROR  (-1)
#endif

typedef enum
{
	MIPI_0=0,
	MIPI_1,
	MIPI_BUTT
}MIPI_CTRL_ENUM;

#define	MIPI_WRITE  (0x01)
#define	MIPI_READ   (0x02)
#define	MIPI_EXTENDED_WRITE (0x03)
#define	MIPI_EXTENDED_READ  (0x04)

#if defined(__VXWORKS__) /* vxworks */
#define  mipi_print_error(fmt, ...)   (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_MIPI, "[mipi]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  mipi_print_info(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_MIPI, "[mipi]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  mipi_print_debug(fmt, ...)   (bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MODU_MIPI, fmt, ##__VA_ARGS__))
#elif defined(__M3_OS__) /* rtx(cm3 os) */
#include <cmsis_os.h>
#include <printk.h>
#include <string.h>
#define  mipi_print_error(fmt, ...)    (printk("[mipi]: <%s> "fmt"\n", __FUNCTION__, ##__VA_ARGS__))
#define  mipi_print_info                printk
#endif
/********************************************************************************/
/*    bbp_mipi 函数（项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/

extern int bsp_mipi_data_rev(u8 type, u8 slave_addr, u8 reg_addr,u8 *data,MIPI_CTRL_ENUM mipi_id);


extern int bsp_mipi_data_send(u8 type, u8 slave_addr, u8 reg_addr,u8 data,MIPI_CTRL_ENUM mipi_id);
extern int bsp_mipi_init(void);

