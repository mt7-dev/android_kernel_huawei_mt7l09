
#ifndef _DRV_MAILBOX_PORT_VXWORKS_H_
#define _DRV_MAILBOX_PORT_VXWORKS_H_


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <stdio.h>
#include "bsp_om.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*虚实地址转换接口定义*/
/*CCPU实地址向虚拟地址映射的转换接口*/
#define MEM_CORE_SHARE_PHY2VIRT(phy)                (phy)

#define MEM_CORE_SHARE_VIRT2PHY(virt)               (virt)

/*邮箱通道，功能，核等相关配置接口定义*/
/*表明此CPU的ID*/
#define MAILBOX_LOCAL_CPUID                         MAILBOX_CPUID_CCPU
 
/*定义C核平台相关的 邮箱通道 总数，关心收发通道 */
#define MAILBOX_CHANNEL_NUM \
    ( MAILBOX_CHANNEL_BUTT(CCPU, HIFI)   \
    + MAILBOX_CHANNEL_BUTT(HIFI, CCPU))

/*定义C核平台相关的 邮件 总数 , 只关心接收通道*/
#define MAILBOX_USER_NUM    \
    ( MAILBOX_USER_BUTT(HIFI, CCPU, MSG)) 

/*打印输出相关配置定义*/

/*控制台打印输出接口*/
#ifdef _DRV_LLT_
#define mailbox_out(p)                             (printf p)
#else
#define mailbox_out(p)                             (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_HIFIMBX, (void*)p))
#endif
/*定义宏控制是否打印行数*/
#define _MAILBOX_LINE_                              __LINE__

/*定义宏控制是否打印文件名,(不打印文件名可节省代码空间)*/
#define _MAILBOX_FILE_                              (void*)(0) /*__FILE__*/   

#define MAILBOX_LOG_LEVEL                           MAILBOX_LOG_INFO                     

/*邮箱调试功能相关配置定义*/
/*表明在此CPU上打开可维可测功能*/
#ifndef MAILBOX_OPEN_MNTN
#define MAILBOX_OPEN_MNTN
#endif

/*可维可测记录的最大传送ID数*/
#define MAILBOX_RECORD_USEID_NUM                    (64)

#define RT  "\n"   /*打印回车符号*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /*_DRV_MAILBOX_VXWORKS_H_*/

