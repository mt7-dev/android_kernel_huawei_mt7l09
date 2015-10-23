/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  The header file exposes all mono to stereo lib APIs
 *
 *	Author: Zesen Zhuang
 *
 *$Id: //srstech/srs_common/std_fxp/include/srs_monotostereo_api.h#4 $
 *$Author: oscarh $
 *$Date: 2010/09/21 $
********************************************************************************/


#ifndef __SRS_MONOTOSTEREO_API_H__
#define __SRS_MONOTOSTEREO_API_H__

#include "srs_typedefs.h"
#include "srs_fxp.h"
#include "srs_monotostereo_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

int				SRS_MonoToStereo_GetObjSize();
SRSResult		SRS_MonoToStereo_CreateObj(SRSMonoToStereoObj *pObj, void *pBuf, SRS_Allocator alloc,void* pContext);

SRSResult		SRS_MonoToStereo_InitObj(SRSMonoToStereoObj m2stereoObj);

SRSResult		SRS_MonoToStereo_Process(SRSMonoToStereoObj m2stereoObj, SRSInt32 *input, SRSStereoCh *output, int blockSamples);

SRSResult		SRS_MonoToStereo_SetInputGain(SRSMonoToStereoObj m2stereoObj, SRSInt16 gain); //gain iwl: SRS_MONOTOSTEREO_GAIN_IWL
SRSInt16		SRS_MonoToStereo_GetInputGain(SRSMonoToStereoObj m2stereoObj);


#ifdef __cplusplus
}
#endif /*__cplusplus*/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif /*__SRS_MONOTOSTEREO_API_H__*/
