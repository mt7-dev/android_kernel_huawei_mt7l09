/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SOS cascaded IIR filter APIs
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id$
 *  $Author$
 *  $Date$
********************************************************************************/
#ifndef __SRS_IIR_API_H__
#define __SRS_IIR_API_H__

#include "srs_iir_def.h"

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

/*APIs*/
int				SRS_IIR_GetObjSize(int maxOrder);

//SRS_IIR_CreateObj: Create an IIR object, the maximum order which can support is 'maxOrder'. The actual order and filter coefficients will be specified
//					 on initialization.
SRSResult		SRS_IIR_CreateObj(SRSIirObj *pObj, void* pBuf, SRS_Allocator alloc, void* pContext, int maxOrder);

//Initialize the filter state to 0. Controls will be intact.Can be called to reinitialize.
//filterCoefs: the memory space pointed to will be referenced throughout the lifecycle of the IIR object. So to use a new set of coefficients,
//             you can simple replace the coefficients in the memory block pointed to by filterCoefs.
SRSResult		SRS_IIR_InitObj(SRSIirObj iirObj, int order, const void *filterCoefs, SRSFilterWl filterWl);

void			SRS_IIR_SetControlDefaults(SRSIirObj iirObj);
void			SRS_IIR_SetEnable(SRSIirObj iirObj, SRSBool enable);

SRSBool			SRS_IIR_GetEnable(SRSIirObj iirObj);

void			SRS_IIR_Process(SRSIirObj iirObj, SRSInt32 *audioIO, int blockSamples);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif //__SRS_IIR_API_H__
