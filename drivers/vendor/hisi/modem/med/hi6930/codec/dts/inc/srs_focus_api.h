/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  FOCUS public APIs header file
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id: //srstech/srs_common/std_fxp/include/srs_focus_api.h#3 $
 *  $Author: oscarh $
 *  $Date: 2010/09/21 $
********************************************************************************/

#ifndef __SRS_FOCUS_API_H__
#define __SRS_FOCUS_API_H__

#include "srs_typedefs.h"
#include "srs_fxp.h"
#include "srs_focus_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

int			SRS_Focus_GetObjSize();
SRSResult	SRS_Focus_CreateObj(SRSFocusObj *pObj, void *pBuf, SRS_Allocator alloc,void* pContext);

SRSResult	SRS_Focus_InitObj8k(SRSFocusObj focusObj);	
SRSResult	SRS_Focus_InitObj11k(SRSFocusObj focusObj);
SRSResult	SRS_Focus_InitObj16k(SRSFocusObj focusObj);
SRSResult	SRS_Focus_InitObj22k(SRSFocusObj focusObj);
SRSResult	SRS_Focus_InitObj24k(SRSFocusObj focusObj);
SRSResult	SRS_Focus_InitObj32k(SRSFocusObj focusObj);
SRSResult	SRS_Focus_InitObj44k(SRSFocusObj focusObj);
SRSResult	SRS_Focus_InitObj48k(SRSFocusObj focusObj);
SRSResult	SRS_Focus_InitObj88k(SRSFocusObj focusObj);
SRSResult	SRS_Focus_InitObj96k(SRSFocusObj focusObj);

void		SRS_Focus_SetControlDefaults(SRSFocusObj focusObj);

void		SRS_Focus_FilterProcess(SRSFocusObj focusObj, SRSInt32 *audioIO, int blockSamples);
void		SRS_Focus_Process(SRSFocusObj focusObj, SRSInt32 *audioIO, int blockSamples, void *ws);

//"Set" functions:
void		SRS_Focus_SetEnable(SRSFocusObj focusObj, SRSBool enable);
SRSResult 	SRS_Focus_SetInputGain(SRSFocusObj focusObj, SRSInt16 gain);
SRSResult 	SRS_Focus_SetOutputGain(SRSFocusObj focusObj, SRSInt16 gain);
SRSResult 	SRS_Focus_SetBypassGain(SRSFocusObj focusObj, SRSInt16 gain);
SRSResult 	SRS_Focus_SetLevel(SRSFocusObj focusObj, SRSInt16 level);

//"Get" functions:
SRSBool		SRS_Focus_GetEnable(SRSFocusObj focusObj);
SRSInt16 	SRS_Focus_GetInputGain(SRSFocusObj focusObj);
SRSInt16 	SRS_Focus_GetOutputGain(SRSFocusObj focusObj);
SRSInt16 	SRS_Focus_GetBypassGain(SRSFocusObj focusObj);
SRSInt16 	SRS_Focus_GetLevel(SRSFocusObj focusObj);

#ifdef __cplusplus
}
#endif /*__cplusplus*/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif //__SRS_FOCUS_API_H__
