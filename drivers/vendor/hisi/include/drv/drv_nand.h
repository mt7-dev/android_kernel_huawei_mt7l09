/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_NAND_H__
#define __DRV_NAND_H__

#include "drv_comm.h"


/******************************************************************************
*
  函数名:       BSP_S32 nand_get_bad_block (BSP_U32 *len, BSP_U32 **ppBadBlock)
  函数描述:     查询整个NAND的所有FLASH 坏块
  输入参数:     无
  输出参数:     pNum       : 返回坏块个数
                ppBadBlock ：数组指针，返回所有坏块的index索引
  返回值:       0    : 查询成功
                负数 : 查询失败
*******************************************************************************/
static INLINE BSP_S32 NAND_GET_BAD_BLOCK(BSP_U32 *pNum, BSP_U32 **ppBadBlock)
{
    return 0;
}

/******************************************************************************
*
  函数名:       BSP_VOID  nand_free_bad_block_mem(BSP_U32* pBadBlock)
  函数描述:     通过nand_get_bad_block接口申请的坏块内存由底软申请，由协议栈调用
                该接口释放。
  输入参数:     pBadBlock
  输出参数:     无
  返回值:       BSP_VOID
*******************************************************************************/
static INLINE BSP_VOID NAND_FREE_BAD_BLOCK_MEM(BSP_U32* pBadBlock)
{
}

#define NAND_MFU_NAME_MAX_LEN    16
#define NAND_DEV_SPEC_MAX_LEN    32

typedef struct
{
    BSP_U32           MufId;                                         /* 厂商ID */
    BSP_U8      aucMufName[NAND_MFU_NAME_MAX_LEN];             /* 厂商名称字符串 */
    BSP_U32           DevId;                                         /* 设备ID */
    BSP_U8      aucDevSpec[NAND_DEV_SPEC_MAX_LEN];             /* 设备规格字符串 */
}NAND_DEV_INFO_S;

/******************************************************************************
*
  函数名:       BSP_S32 nand_get_dev_info (NAND_DEV_INFO_S *pNandDevInfo)
  函数描述:     查询NAND设备相关信息：包括厂商ID、厂商名称、设备ID、设备规格
  输入参数:     无
  输出参数:     pNandDevInfo    存储NAND设备相关信息的结构体
  返回值:       0    : 查询成功
                负数 : 查询失败
*******************************************************************************/
static INLINE BSP_S32 NAND_GET_DEV_INFO(NAND_DEV_INFO_S *pNandDevInfo)
{
    return 0;
}

/************************************************************************
 * FUNCTION
 *       max_freeblock_size_get
 * DESCRIPTION
 *       get memory max free block size
 * INPUTS
 *       无
 * OUTPUTS
 *       max free block size
 *************************************************************************/
static INLINE int DRV_GET_FREE_BLOCK_SIZE(void)
{
	return 0;
}

/********************************************************************************************************
 函 数 名  : BSP_FS_GetDskspc
 功能描述  : 得到文件系统空间。
 输入参数  : path：文件系统路径。
 输出参数  : DskSpc：总空间。
                           UsdSpc：用过空间。
                           VldSpc：空闲空间。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
static INLINE int BSP_FS_GetDskspc(const char *path,unsigned int *DskSpc,unsigned int  *UsdSpc,  unsigned int *VldSpc)
{
	return 0;
}
#define DRV_FILE_GET_DISKSPACE(path,DskSpc,UsdSpc,VldSpc)    BSP_FS_GetDskspc(path,DskSpc,UsdSpc,VldSpc)


/*****************************************************************************
 函 数 名  : BSP_MNTN_GetFlashSpec
 功能描述  : Get flash infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
static INLINE int BSP_MNTN_GetFlashSpec(unsigned char * pFlashInfo, unsigned int ulLength)
{
	return 0;
}
#define DRV_GET_FLASH_INFO(pFlashInfo, usLength)    BSP_MNTN_GetFlashSpec (pFlashInfo, usLength )
#endif

