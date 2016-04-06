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


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/
unsigned int mailbox_send_msg(
                unsigned int            mailcode,
                void                    *data,
                unsigned int            length);

unsigned int mailbox_reg_msg_cb(
                unsigned int             mailcode,
                mb_msg_cb                 func,
                void                     *data);

unsigned int mailbox_read_msg_data(
                void                   *mail_handle,
                 char                  *buff,
                unsigned int          *size);
/*****************************************************************************
  3 函数实现
*****************************************************************************/


unsigned int DRV_MAILBOX_SENDMAIL(
                unsigned int           MailCode,
                void                   *pData,
                unsigned int           Length)
{
    return mailbox_send_msg(MailCode, pData, Length);
}


unsigned int DRV_MAILBOX_REGISTERRECVFUNC(
                unsigned int           MailCode,
                mb_msg_cb               pFun,
                void                   *UserHandle)
{
    return mailbox_reg_msg_cb(MailCode, pFun, UserHandle);
}


unsigned int DRV_MAILBOX_READMAILDATA(
                void                   *MailHandle,
                unsigned char          *pData,
                unsigned int          *pSize)
{
    return mailbox_read_msg_data(MailHandle, (char *)pData, pSize);
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

