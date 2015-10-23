/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  bsp_efuse.h
*
*   作    者 :  liuwenhui
*
*   描    述 :  bsp_efuse 的头文件
*
*   修改记录 :  2013年12月23日  v1.00  liuwenhui  创建
*
*************************************************************************/

#ifndef __BSP_EFUSE_H__
#define __BSP_EFUSE_H__

#include "osl_types.h"

#define EFUSE_OK               (0)
#define EFUSE_ERROR            (-1)

#define EFUSE_GROUP_SIZE        (4)
#define EFUSE_MAX_ALLOW_SIZE    (16)

#define  efuse_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_EFUSE, "[efuse]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  efuse_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_EFUSE, "[efuse]: "fmt, ##__VA_ARGS__))


typedef enum
{
    EFUSE_READ,
    EFUSE_WRITE    

}EFUSE_OPT;

typedef struct
{
    EFUSE_OPT opt;
    u32 start;
    u32 len;
    int ret;
    u32 buf[EFUSE_MAX_ALLOW_SIZE];
}EFUSE_DATA_STRU;

/*****************************************************************************
* 函 数 名  : bsp_efuse_read
*
* 功能描述  : 按组读取EFUSE中的数据
*
* 输入参数  : group  起始group
*                   num  数组长度(word数,不超过16)
* 输出参数  : pBuf ：EFUSE中的数据
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
int bsp_efuse_read(u32* pBuf, const u32 group, const u32 num);

/*****************************************************************************
* 函 数 名  : bsp_efuse_write
*
* 功能描述  : 烧写Efsue
*
* 输入参数  : pBuf:待烧写的EFUSE值
*                 group,Efuse地址偏移
*                 len,烧写长度
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
int bsp_efuse_write( u32 *pBuf, const u32 group, const u32 len);

/*****************************************************************************
* 函 数 名  : bsp_efuse_show
*
* 功能描述  : 显示efsue各组信息
*
* 输入参数  : 
* 
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void bsp_efuse_show(void);

#endif
