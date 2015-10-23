/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  ParametricEQ types, constants
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id: //srstech/srs_common/std_fxp/include/srs_parametriceq_def.h#2 $
 *  $Author: oscarh $
 *  $Date: 2011/02/14 $
********************************************************************************/
#ifndef __SRS_PARAMETRICEQ_DEF_H__
#define __SRS_PARAMETRICEQ_DEF_H__

#include "srs_typedefs.h"

typedef struct _SRSPeqObj *SRSPeqObj;

#define SRS_PEQ_GAIN_IWL			4	//Integer word length of inpug gain, output gain and bypass gain

typedef struct
{
	int						NumOfBands;		//The number of bands of the TruEQ object
	const void				*FilterCoefs;	//The filter coeffcients of the TruEQ object, could be SRSInt32* or SRSInt16*
	SRSFilterWl				FilterWl;		//Filter coefficient word length, could be SRSFilter32 or SRSFilter16
} SRSPeqFilterCfg;

typedef struct
{
	int MaxNumOfBands;
} SRSPeqCreateData;

typedef struct
{
	SRSBool		Enable;
	SRSInt16	InputGain;
	SRSInt16	OutputGain;
	SRSInt16	BypassGain;
}SRSPeqControls;

#endif //__SRS_PARAMETRICEQ_DEF_H__
