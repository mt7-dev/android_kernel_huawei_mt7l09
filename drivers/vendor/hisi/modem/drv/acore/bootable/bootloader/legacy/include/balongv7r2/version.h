#ifndef VERSION_H
#define VERSION_H

#include <balongv7r2/types.h>
#include "drv_version.h"
/*****************************************************************************
* 函 数 名  	: bsp_save_hw_version
*
* 功能描述  : 向AXI MEMORY中存储hw_id
*
* 输入参数  : 无
* 输出参数  :
*
* 返 回 值  	:
				0:成功
				-1:失败
* 修改记录  :
*
*****************************************************************************/
int bsp_save_hw_version(void);
/*****************************************************************************
* 函数	: bsp_board_type_is_gutl
* 功能	: get board type is GUTL
* 输入	: void
* 输出	: void
* 返回	: BOARD_TYPE_E
*****************************************************************************/
bool bsp_board_type_is_gutl(void);

/*****************************************************************************
* 函数	: bsp_get_board_chip_type
* 功能	: get board type and chip type
* 输入	: void
* 输出	: void
* 返回	: BOARD_TYPE_E
*****************************************************************************/
u32 bsp_version_get_board_chip_type(void);

#endif
