/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileNmae: \vos\include\vos\v_id.inc                                       */
/*                                                                           */
/* Author: Qin Peifeng                                                       */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2000-04-21                                                          */
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2000-04-21                                                       */
/*    Author: Qin Peifeng                                                    */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef _V_ID_INC
#define _V_ID_INC

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

extern VOS_UINT32 MOD_RegFidPidRSP( VOS_UINT32 ulPID, VOS_UINT32 ulFID);

/*********************** INNER VARIABLES DEFINITION **************************/
/* PID与FID对应关系表 */
BEGIN_DECLARE_PID_BELONG_TO_FID()
    ON_DECLARE(DSP_PID_VOICE_RT,                    DSP_FID_RT)
    ON_DECLARE(DSP_PID_AUDIO_RT,                    DSP_FID_RT)
    ON_DECLARE(DSP_PID_VOICE,                       DSP_FID_NORMAL)
    ON_DECLARE(DSP_PID_AUDIO,                       DSP_FID_LOW)
    ON_DECLARE(DSP_PID_HIFI_OM,                     DSP_FID_LOW)

END_DECLARE_PID_BELONG_TO_FID()

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_ID_INC */
