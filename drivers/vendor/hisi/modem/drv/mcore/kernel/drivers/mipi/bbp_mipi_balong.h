/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : apb_mipi_balong.h */
/* Version       : 2.0 */
/* Created       : 2013-03-13*/
/* Last Modified : */
/* Description   :  The C union definition file for the module apb_mipi*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
#ifndef BBP_MIPI_BALONG_H
#define BBP_MIPI_BALONG_H
#include "osl_types.h"
#include "bsp_mipi.h"

#define LOW_32BIT_MASK         (0xFFFFFFFF)
#define MIPI_GRANT_FLAG_IS_HIGH (0x1)
#define MIPI_REG_WAIT_TIMEOUT (1000000)
#define MIPI_IS_INITIALIZED_FLAG (0x123)

#define MIPI_PASTAR_ADDR 5
#define DSP_MIPI0_EN_IMI 0x00f8
#define DSP_MIPI0_EN_IMI_MASK ~(1<<0)
#define MIPI_GRANT_DSP_FLAG_IS_HIGH 1
#define RD_END_FLAG_MIPI_SOFT_FLAG_IS_HIGH 1
#define MIPI_EXTENDED_WR_DATA_FRAME_SIZE 8
#define MIPI_TASK_DELAY_TICKS	2
#define MIPI_READ_END_FLAG 1	/*only read has end_flag*/
#define MIPI_CMD_TYPE_READ		2
#define MIPI_CMD_TYPE_WRITE		0
#define MIPI_WRITE_END_FLAG		0
#define BIT_CNT					8	/*bit number in a byte*/

#define MIPI_HIGH_OFFSET				32
#define MIPI_CMD_TYPE_PARITY1_OFFSET	  4

/*config frame*/
#define MIPI_TRANS_TYPE_OFFSET	61
#define MIPI_SLAVE_ADDR_OFFSET	57
#define MIPI_CMD_TYPE_OFFSET		53
#define MIPI_BYTE_CNT_OFFSET		49
#define MIPI_PARITY1_OFFSET		48
#define MIPI_REG_ADDR_OFSET		40
#define MIPI_PARITY2_OFFSET		39
#define MIPI_END_FLAG_OFFSET		34
#define MIPI_DATA_OFFSET			31
#define MIPI_PARITY3_OFFSET		30

#define MIPI_EXTENDED_WR_DATA_FRAME_MASK    (((u32)(0x1)<<MIPI_EXTENDED_WR_DATA_FRAME_SIZE)-1)
#define MIPI_EXTENDED_WR_FIRST_DATA_FRAME_GET(data_high, data_low)  \
(((data_high&MIPI_EXTENDED_WR_DATA_FRAME_MASK)<<1)|((data_low>>31)&0x1))


/*类型定义*/
typedef struct
{
	u32 mipi_cmd_low;
	u32 mipi_cmd_high;
}MIPI_CMD_STRU;

typedef enum
{
	MIPI_WRITE = 0x1,
	MIPI_READ,
	MIPI_EXTENDED_WRITE ,
	MIPI_EXTENDED_READ

}MIPI_CMD_TYPE_ENUM;

typedef struct
{
	u32 en_pastar;
	u32 slave_addr;
	u32 wr_cmd_type;
	u32	rd_cmd_type;
	u32	byte_cnt;
	u32	advance_time;
}MIPI_INIT_STRU;


#ifndef INLINE
#define INLINE inline
#endif
#define HI_SET_GET(__full_name__,__reg_name,__reg_type, __reg_base, __reg_offset)  \
static INLINE void set_##__full_name__(unsigned int ctrl_id,unsigned int val)  \
{\
	__reg_type	reg_obj; \
	reg_obj.u32 = readl((__reg_base[ctrl_id])+ __reg_offset); \
	reg_obj.bits.__reg_name = val; \
	writel(reg_obj.u32, (__reg_base[ctrl_id])+ __reg_offset); \
} \
static INLINE unsigned int get_##__full_name__(unsigned int ctrl_id)  \
{\
	__reg_type	reg_obj; \
	reg_obj.u32 = readl((__reg_base[ctrl_id])+ __reg_offset); \
	return reg_obj.bits.__reg_name; \
}

#endif
