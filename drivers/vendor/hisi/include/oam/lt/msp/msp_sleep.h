//*****************************************************************************
//  文 件 名   : msp_drx.h
//  版 本 号   : V1.0
//  修改历史   :
//    修改内容 : 创建文件
//*****************************************************************************
#ifndef __MSP_DRX_H__
#define __MSP_DRX_H__

#include "vos.h"
#include <DrvInterface.h>

extern VOS_BOOL MSP_IsSleepReady_Acpu(VOS_VOID);
extern VOS_BOOL MSP_IsSleepReady_Mcpu(VOS_VOID);
extern VOS_VOID TLSLEEP_ActivateHw(PWC_COMM_MODE_E modeid);

#endif


