/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_mailbox.c
  版 本 号   : 初稿
  作    者   : 苏庄銮 59026
  生成日期   : 2012年12月15日
  最近修改   :
  功能描述   : mailbox接口
  函数列表   :

  修改历史   :
  1.日    期   : 2012年12月15日
    作    者   : 苏庄銮 59026
    修改内容   : 创建文件
******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "drv_mailbox.h"
#include "drv_mailbox_cfg.h"
#include "bsp_nvim.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef BSP_CONFIG_HI3630
#ifdef CONFIG_HIFI


/*****************************************************************************
  2 函数声明
*****************************************************************************/
unsigned long mailbox_send_msg(
                unsigned long            mailcode,
                void                    *data,
                unsigned long            length);

unsigned long mailbox_reg_msg_cb(
                unsigned long             mailcode,
                mb_msg_cb                 func,
                void                     *data);

unsigned long mailbox_read_msg_data(
                void                   *mail_handle,
                 char                  *buff,
                unsigned long          *size);
/*****************************************************************************
  3 函数实现
*****************************************************************************/


unsigned long DRV_MAILBOX_SENDMAIL(
                unsigned long           MailCode,
                void                   *pData,
                unsigned long           Length)
{
    return mailbox_send_msg(MailCode, pData, Length);
}


unsigned long DRV_MAILBOX_REGISTERRECVFUNC(
                unsigned long           MailCode,
                mb_msg_cb               pFun,
                void                   *UserHandle)
{
    return mailbox_reg_msg_cb(MailCode, pFun, UserHandle);
}


unsigned long DRV_MAILBOX_READMAILDATA(
                void                   *MailHandle,
                unsigned char          *pData,
                unsigned long          *pSize)
{
    return mailbox_read_msg_data(MailHandle, (char *)pData, pSize);
}

#define HIFI_MBX_OFFSET    0xa0000000
/*****************************************************************************
 函 数 名  : drv_hifi_fill_mb_info
 功能描述  : 邮箱信息设置接口
 输入参数  : addr 段信息地址(由HIFI加载模块传入)
 输出参数  : 
 返 回 值  : void
 调用函数  : V9R1中在fastboot中调用，V7R2中与DSP一起加载调用
 被调函数  :

 修改历史      :
  1.日    期   : 2013年11月4日
    作    者   : 崔军强 64416
    修改内容   : 新生成函数

*****************************************************************************/
void drv_hifi_fill_mb_info(unsigned int addr)
{
    CARM_HIFI_DYN_ADDR_SHARE_STRU *pdata;

    pdata = (CARM_HIFI_DYN_ADDR_SHARE_STRU*)addr;
    
    pdata->stCarmHifiMB.uwHifi2CarmMailBoxLen   = MAILBOX_QUEUE_SIZE(HIFI, CCPU, MSG);
    pdata->stCarmHifiMB.uwCarm2HifiMailBoxLen   = MAILBOX_QUEUE_SIZE(CCPU, HIFI, MSG);
    pdata->stCarmHifiMB.uwHifiCarmHeadAddr      = HIFI_MBX_OFFSET + MAILBOX_HEAD_ADDR(HIFI, CCPU, MSG);
    pdata->stCarmHifiMB.uwHifiCarmBodyAddr      = HIFI_MBX_OFFSET + MAILBOX_QUEUE_ADDR(HIFI, CCPU, MSG);
    pdata->stCarmHifiMB.uwCarmHifiHeadAddr      = HIFI_MBX_OFFSET + MAILBOX_HEAD_ADDR(CCPU, HIFI, MSG);
    pdata->stCarmHifiMB.uwCarmHifiBodyAddr      = HIFI_MBX_OFFSET + MAILBOX_QUEUE_ADDR(CCPU, HIFI, MSG);
    pdata->stCarmHifiMB.uwProtectWord           = HIFI_MB_ADDR_PROTECT;

    pdata->stAarmHifiMB.uwHifi2AarmMailBoxLen   = MAILBOX_QUEUE_SIZE(HIFI, ACPU, MSG);
    pdata->stAarmHifiMB.uwAarm2HifiMailBoxLen   = MAILBOX_QUEUE_SIZE(ACPU, HIFI, MSG);
    pdata->stAarmHifiMB.uwHifiAarmHeadAddr      = HIFI_MBX_OFFSET + MAILBOX_HEAD_ADDR(HIFI, ACPU, MSG);
    pdata->stAarmHifiMB.uwHifiAarmBodyAddr      = HIFI_MBX_OFFSET + MAILBOX_QUEUE_ADDR(HIFI, ACPU, MSG);
    pdata->stAarmHifiMB.uwAarmHifiHeadAddr      = HIFI_MBX_OFFSET + MAILBOX_HEAD_ADDR(ACPU, HIFI, MSG);
    pdata->stAarmHifiMB.uwAarmHifiBodyAddr      = HIFI_MBX_OFFSET + MAILBOX_QUEUE_ADDR(ACPU, HIFI, MSG);
    pdata->stAarmHifiMB.uwProtectWord           = HIFI_MB_ADDR_PROTECT;
    
    pdata->uwNvBaseAddrPhy = HIFI_MBX_OFFSET + SHD_DDR_V2P(NV_GLOBAL_CTRL_INFO_ADDR);
    pdata->uwNvBaseAddrVirt = HIFI_MBX_OFFSET + NV_GLOBAL_CTRL_INFO_ADDR;
    
    pdata->uwProtectWord = HIFI_MB_ADDR_PROTECT;
    
    return ;
}
#else

unsigned long DRV_MAILBOX_SENDMAIL(
                unsigned long           MailCode,
                void                   *pData,
                unsigned long           Length)
{
    return MAILBOX_OK;
}


unsigned long DRV_MAILBOX_REGISTERRECVFUNC(
                unsigned long           MailCode,
                mb_msg_cb               pFun,
                void                   *UserHandle)
{
    return MAILBOX_OK;
}


unsigned long DRV_MAILBOX_READMAILDATA(
                void                   *MailHandle,
                unsigned char          *pData,
                unsigned long          *pSize)
{
    *pSize = 0;
    return MAILBOX_OK;
}
/*****************************************************************************
 函 数 名  : drv_hifi_fill_mb_info
 功能描述  : 邮箱信息设置接口
 输入参数  : addr 段信息地址(由HIFI加载模块传入)
 输出参数  : 
 返 回 值  : void
 调用函数  : V9R1中在fastboot中调用，V7R2中与DSP一起加载调用
 被调函数  :

 修改历史      :
  1.日    期   : 2013年11月4日
    作    者   : 崔军强 64416
    修改内容   : 新生成函数

*****************************************************************************/
void drv_hifi_fill_mb_info(unsigned int addr)
{
    return ;
}
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

