

/******************************************************************************
 PROJECT   :
 SUBSYSTEM :
 MODULE    :
 OWNER     :
******************************************************************************/


#ifndef __DMS_CORE_H__
#define __DMS_CORE_H__

#include "dms.h"
#include "vos.h"
#if (VOS_OS_VER == VOS_LINUX)
#include "linux/module.h"
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/bitops.h>
#else
#include "Linuxstub.h"
#endif

#define DMS_WRT_SLEEP_TIME              (50)
#define DMS_RD_SLEEP_TIME               (50)
#define DMS_RD_BUF_LEN                  (1024)

#define DMS_VFILE_CRT_LEVEL             (0770)

#define DMS_TEST_MODE                   (0)
#define DMS_NORMAL_MODE                 (1)
#define DMS_PORTCFG_FILE_LEN            (4)

#define DMS_APP_DS_TASK_PRIORITY        (84)

/* 最后使能的逻辑通道和物理通道的对应关系
 */
typedef struct
{
    DMS_PHY_BEAR_ENUM aenPhyChan;
} DMS_LOGIC_PHY_MATCH_STRU;

/* 物理通道所对应的逻辑属性结构体
 */
typedef struct
{
    VOS_INT32  slPortHandle;
    DMS_LOGIC_CHAN_ENUM aenLogicChan;
    VOS_UINT8 ucChanStat;
    VOS_UINT8 ucHdlcFlag;                 /*0 标示非HDLC数据，1表示是HDLC数据
 */
    VOS_UINT8 ucReserve[2];
    VOS_UINT32 ulDecodeSrcChanId;/*解码源通道ID
 */
    VOS_UINT32 ulCodeDesChanId;   /*编码目的通道ID
 */
    VOS_UINT32  ulRecivBufLen;
} DMS_PHY_BEAR_PROPERTY_STRU;


typedef struct
{
    DMS_READ_DATA_PFN                   pfnRdDtaCallback;
    DMS_CONNECT_STA_PFN                 pfnConnectCallBack;
    DMS_LOGIC_PHY_MATCH_STRU            stLogicPhy[EN_DMS_CHANNEL_LAST];
    DMS_PHY_BEAR_PROPERTY_STRU          stPhyProperty[EN_DMS_BEARER_LAST];
    VOS_BOOL                            ucDmsVcom1SleepFlag ;
    VOS_BOOL                            ucDmsVcomATSleepFlag ;
    VOS_BOOL                            ucDmsVcomUartSleepFlag ;
    VOS_BOOL                            bPortCfgFlg;
    VOS_BOOL                            bPortOpenFlg;

    VOS_UINT32                          ulPortCfgValue;
} DMS_MAIN_INFO;



extern  DMS_PHY_BEAR_PROPERTY_STRU* dmsgetPhyBearProperty(VOS_VOID);
extern DMS_MAIN_INFO * dmsGetMainInfo(VOS_VOID);

extern VOS_VOID dms_AtNdisWrtCB (char* pDoneBuff, int status);

VOS_UINT32 DMS_InitPorCfgFile(VOS_VOID);

ssize_t DMS_ReadPortCfgFile(struct file  *file,
                                   char __user  *buf,
                                   size_t        len,
                                   loff_t       *ppos);
ssize_t DMS_WritePortCfgFile(struct file        *file,
                                   const char __user  *buf,
                                   size_t              len,
                                   loff_t             *ppos);

#endif /*__DMS_CORE_H__ */


