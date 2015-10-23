/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : MnMsgInclude.h
  版 本 号   : 初稿
  作    者   : 周君 40661
  生成日期   : 2008年2月15日
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2008年2月15日
    作    者   : 周君 40661
    修改内容   : 创建文件

******************************************************************************/
#ifndef __MNMSGINCLUDE_H__
#define __MNMSGINCLUDE_H__


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "product_config.h"
#include  "vos.h"
#include  "Ps.h"
#include  "NasSms.h"
#include  "NasOmInterface.h"
#include  "NasOmTrans.h"
#include  "MnComm.h"
#include  "MnErrorCode.h"
/* 删除ExtAppMmcInterface.h*/

#include  "TafMmcInterface.h"
#include  "MnMsgExt.h"
#include  "MnMsgApi.h"
#include  "MnMsgSmCommProc.h"
#include  "MnMsgSendSms.h"
#include  "MnMsgTimerProc.h"
#include  "UsimPsInterface.h"
#include  "NVIM_Interface.h"
#include  "MnMsgUsimProc.h"
#include  "MnMsgTs.h"
#include  "MnMsgReport.h"
#include  "UsimPsApi.h"
#include  "NasComm.h"

#include  "TafCbsInterface.h"

#if (FEATURE_IMS == FEATURE_ON)
#include  "MsgImsaInterface.h"
#endif

#endif /* __MNMSGINCLUDE_H__ */

