/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SRS Definition public APIs header file
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id: //srstech/srs_common/std_fxp/include/srs_definition_api.h#4 $
 *  $Author: oscarh $
 *  $Date: 2010/09/21 $
********************************************************************************/

#ifndef __SRS_DEFINITION_API_H__
#define __SRS_DEFINITION_API_H__

#include "srs_typedefs.h"
#include "srs_fxp.h"
#include "srs_def_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

int				SRS_Def_GetObjSize();
SRSResult		SRS_Def_CreateObj(SRSDefObj *pObj, void *pBuf, SRS_Allocator alloc,void* pContext);

SRSResult		SRS_Def_InitObj8k(SRSDefObj defObj);	
SRSResult		SRS_Def_InitObj11k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj16k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj22k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj24k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj32k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj44k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj48k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj88k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj96k(SRSDefObj defObj);

void			SRS_Def_SetControlDefaults(SRSDefObj defObj);

void		SRS_Def_FilterProcess(SRSDefObj defObj, SRSInt32 *audioIO, int blockSamples);
void		SRS_Def_Process(SRSDefObj defObj, SRSInt32 *audioIO, int blockSamples, void *ws);

//"Set" functions:
void		SRS_Def_SetEnable(SRSDefObj defObj, SRSBool enable);
SRSResult	SRS_Def_SetInputGain(SRSDefObj defObj, SRSInt16 gain);
SRSResult	SRS_Def_SetOutputGain(SRSDefObj defObj, SRSInt16 gain);
SRSResult	SRS_Def_SetBypassGain(SRSDefObj defObj, SRSInt16 gain);
SRSResult	SRS_Def_SetLevel(SRSDefObj defObj, SRSInt16 level);

//"Get" functions:
SRSBool		SRS_Def_GetEnable(SRSDefObj defObj);
SRSInt16	SRS_Def_GetInputGain(SRSDefObj defObj);
SRSInt16	SRS_Def_GetOutputGain(SRSDefObj defObj);
SRSInt16	SRS_Def_GetBypassGain(SRSDefObj defObj);
SRSInt16	SRS_Def_GetLevel(SRSDefObj defObj);

#ifdef __cplusplus
}
#endif /*__cplusplus*/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif //__SRS_DEFINITION_API_H__
