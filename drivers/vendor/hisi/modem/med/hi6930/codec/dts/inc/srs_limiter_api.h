/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  Exposes all srs_limiter APIs
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_common/std_fxp/include/srs_limiter_api.h#4 $
 *  $Author: oscarh $
 *  $Date: 2010/09/21 $
 *	
********************************************************************************/

#ifndef __SRS_LIMITER_API_H__
#define __SRS_LIMITER_API_H__

#include "srs_limiter_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

int			SRS_Limiter_GetObjSize();

SRSResult	SRS_Limiter_CreateObj(SRSLimiterObj *pObj, void *pBuf, SRS_Allocator alloc,void* pContext);

SRSResult	SRS_Limiter_InitObj8k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj11k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj16k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj22k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj24k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj32k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj44k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj48k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj88k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj96k(SRSLimiterObj lmtObj);

SRSResult	SRS_Limiter_ProcessMono(SRSLimiterObj lmtObj, SRSInt32 *audioChan, int blockSamples);
SRSResult	SRS_Limiter_ProcessStereo(SRSLimiterObj lmtObj, SRSStereoCh *audioIO, int blockSamples);

SRSResult	SRS_Limiter_SetMinimalGain(SRSLimiterObj lmtObj, SRSInt32 minGain); //limit: I32.2
void		SRS_Limiter_SetEnable(SRSLimiterObj lmtObj, SRSBool enable);

SRSInt32	SRS_Limiter_GetMinimalGain(SRSLimiterObj lmtObj);
SRSBool		SRS_Limiter_GetEnable(SRSLimiterObj lmtObj);

void		SRS_Limiter_SetControlDefaults(SRSLimiterObj lmtObj);


#ifdef __cplusplus
}
#endif /*__cplusplus*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /*__SRS_LIMITER_API_H__*/
