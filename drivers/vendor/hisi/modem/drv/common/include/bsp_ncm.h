/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_ncm.h
*
*   作    者 :  wangzhongshun
*
*   描    述 :  ncm drv内部接口头文件
*s
*   修改记录 :  2013年2月2日  v1.00  wangzhongshun创建
*************************************************************************/

#ifndef __BSP_NCM_H__
#define __BSP_NCM_H__

#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */

#include <linux/skbuff.h>

#include "drv_ncm.h"

typedef struct ncm_vendor_ctx
{
    bool is_mbim_mode;              /* if mbim mode now */
    bool connect;
    NCM_IOCTL_CONNECTION_SPEED_S speed;
    struct completion notify_done;  /* notify completion */
    bool tx_task_run;               /* tx task run */
    struct task_struct *tx_task;    /* tx task */
    struct completion tx_task_stop; /* notify tx task stoped */
    struct sk_buff_head tx_frames;  /* hold tx frames from vendor */
    struct completion tx_kick;      /* kick tx task */
    void *ncm_priv;                 /* hold f_ncm */
    void *vendor_priv;              /* hold app_ctx */

    /* stat info */
    ulong stat_rx_total;
    ulong stat_rx_einval;
    ulong stat_tx_total;
    ulong stat_tx_cancel;
    ulong stat_tx_xmit;
    ulong stat_tx_xmit_fail;
    ulong stat_tx_post;
    ulong stat_tx_drop;
    ulong stat_tx_nochl;
    ulong stat_tx_nodev;
    ulong stat_notify_timeout;
}ncm_vendor_ctx_t;

void* bsp_ncm_open(NCM_DEV_TYPE_E enDevType, BSP_U32 pu32NcmDevId);
BSP_U32 bsp_ncm_write(BSP_VOID * u32NcmDevId, BSP_VOID *pPktEncap, void *net_priv);
BSP_U32 bsp_ncm_ioctl(BSP_VOID * u32NcmDevId, NCM_IOCTL_CMD_TYPE_E enNcmCmd, BSP_VOID *param);
BSP_U32 bsp_ncm_close(NCM_DEV_TYPE_E enDevType, BSP_VOID * u32NcmDevId);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End of __BSP_ACM_H__ */
