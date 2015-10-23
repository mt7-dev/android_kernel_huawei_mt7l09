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



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

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



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

