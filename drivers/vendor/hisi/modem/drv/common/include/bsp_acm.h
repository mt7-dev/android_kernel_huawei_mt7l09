/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_acm.h
*
*   作    者 :  fanchangdong
*
*   描    述 :  ncm drv内部接口头文件
*
*   修改记录 :  2013年2月2日  v1.00  fanchangdong创建
*************************************************************************/

#ifndef __BSP_ACM_H__
#define __BSP_ACM_H__

#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */

extern void* bsp_acm_open(u32 dev_id);
extern s32 bsp_acm_close(void* handle);
extern s32 bsp_acm_write(void* handle, void *buf, u32 size);
extern s32 bsp_acm_read(void* handle, void *buf, u32 size);
extern s32 bsp_acm_ioctl(void* handle, u32 cmd, void *para);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End of __BSP_ACM_H__ */
