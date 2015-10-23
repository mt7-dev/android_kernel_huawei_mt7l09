/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  slave_errcode.h
*
*   作    者 :  xumushui
*
*   描    述 :  slave模块错误码定义
*
*   修改记录 :  2011年8月27日  v1.00  xumushui      创建
*
*************************************************************************/

#ifndef __SLAVE_ERRCODE_H__
#define __SLAVE_ERRCODE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "drv_comm.h"
#include "bsp_om.h"
#define BSP_MODU_SLAVE 11
/* 通用错误码，从0x1001开始 */
#define BSP_ERR_COMMON (0x1000)
#define BSP_ERR_MODULE_NOT_INITED (BSP_ERR_COMMON + 1)
#define BSP_ERR_NULL_PTR (BSP_ERR_COMMON + 2)
#define BSP_ERR_INVALID_PARA (BSP_ERR_COMMON + 3)
#define BSP_ERR_RETRY_TIMEOUT (BSP_ERR_COMMON + 4)
#define BSP_ERR_BUF_ALLOC_FAILED (BSP_ERR_COMMON + 5)
#define BSP_ERR_BUF_FREE_FAILED (BSP_ERR_COMMON + 6)

/* 各模块专用错误码，从0x1开始  */
#define BSP_ERR_SPECIAL (0x0)
/* 通用错误码 */
#define SLAVE_ERR_MODULE_NOT_INITED     BSP_DEF_ERR(BSP_MODU_SLAVE,BSP_ERR_MODULE_NOT_INITED)
#define SLAVE_ERR_NULL_POINTER          BSP_DEF_ERR(BSP_MODU_SLAVE,BSP_ERR_NULL_PTR)
#define SLAVE_ERR_INVALID_PARA          BSP_DEF_ERR(BSP_MODU_SLAVE,BSP_ERR_INVALID_PARA)
#define SLAVE_ERR_MALLOC_FAILED         BSP_DEF_ERR(BSP_MODU_SLAVE,BSP_ERR_BUF_ALLOC_FAILED)
#define SLAVE_ERR_FREE_FAILED           BSP_DEF_ERR(BSP_MODU_SLAVE,BSP_ERR_BUF_FREE_FAILED)
#define SLAVE_ERR_RETRY_TIMEOUT         BSP_DEF_ERR(BSP_MODU_SLAVE,BSP_ERR_RETRY_TIMEOUT)

/* 专用错误码 */
#define SLAVE_ERR_UNKNOWN               BSP_DEF_ERR(BSP_MODU_SLAVE,(BSP_ERR_SPECIAL + 0x1))
#define SLAVE_ERR_INVAL_ENUMCFG         BSP_DEF_ERR(BSP_MODU_SLAVE,(BSP_ERR_SPECIAL + 0x2))
#define SLAVE_ERR_TASK_CREATE           BSP_DEF_ERR(BSP_MODU_SLAVE,(BSP_ERR_SPECIAL + 0x3))
#define SLAVE_ERR_MSG_CREATE            BSP_DEF_ERR(BSP_MODU_SLAVE,(BSP_ERR_SPECIAL + 0x4))
#define SLAVE_ERR_SEM_CREATE            BSP_DEF_ERR(BSP_MODU_SLAVE,(BSP_ERR_SPECIAL + 0x5))
#define SLAVE_ERR_FIFO_FULL             BSP_DEF_ERR(BSP_MODU_SLAVE,(BSP_ERR_SPECIAL + 0x6))
#define SLAVE_ERR_INVAL_CONDITION       BSP_DEF_ERR(BSP_MODU_SLAVE,(BSP_ERR_SPECIAL + 0x7))
#define SLAVE_ERR_BUFF_OVERFLOW         BSP_DEF_ERR(BSP_MODU_SLAVE,(BSP_ERR_SPECIAL + 0x8))

/* Defines for Success and Fail*/
#define SLAVE_OK	    (BSP_OK)
#define SLAVE_ERROR     (BSP_ERROR)

#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_GLOBAL_H_*/
