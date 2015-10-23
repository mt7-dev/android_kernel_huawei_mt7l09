/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SRS CSHP types, contants
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_cshp/std_fxp/include/srs_cshp_def.h#2 $
 *  $Author: oscarh $
 *  $Date: 2011/02/15 $
 *	
********************************************************************************/

#ifndef __SRS_CSHP_DEF_H__
#define __SRS_CSHP_DEF_H__

#include "srs_typedefs.h"
#include "srs_csdecoder_def.h"
#include "srs_def_def.h"
#include "srs_focus_def.h"
#include "srs_limiter_def.h"
#include "srs_hp360_def.h"
#include "srs_tbhd_def.h"


typedef struct _SRSCshpObj *SRSCshpObj;



#define SRS_CSHP_WORKSPACE_SIZE(blockSize)		(SRS_CSHP_MAX(\
													SRS_CSHP_MAX(SRS_CSD_WORKSPACE_SIZE(blockSize), SRS_TBHD_WORKSPACE_SIZE(blockSize)), \
													SRS_HP360_WORKSPACE_SIZE(blockSize) \
												 ) + 6*sizeof(SRSInt32)*(blockSize) + 8)

#define SRS_CSHP_GAIN_IWL	1		//iwl of InputGain, OutputGain and BypassGain

typedef enum
{
	SRS_CSHP_PASSIVE_DECODER,
	SRS_CSHP_CS_DECODER, 
	SRS_CSHP_RENDER_UNUSED = 0x7FFFFFFF //force to generate 32-bit enum
} SRSCshpRenderingMode;

#define SRS_CSHP_MAX(a, b)	((a)>=(b)? (a):(b))

typedef struct
{
	SRSBool				Enable;
	SRSBool				SplitAnalysisEnable;  //Enable/disable the split analysis feature
	SRSInt16			CompressorLevel;	//0.0~1.0
	SRSInt16			Level;	//TruBass Level, 0.0~1.0
	SRSTBHDSpeakerSize	SpkSize; //{AudioFilter, Analysis Filter}
	SRSTBHDMode			Mode;		//Mono mode or stereo mode TB
} SRSCshpTruBassControls;

typedef struct
{
	SRSBool					Enable;
	SRSInt16				InputGain;
	SRSInt16				OutputGain;
	SRSInt16				BypassGain;
	SRSCshpRenderingMode	Render;
	SRSCSDProcMode			CSDProcMode; //CSDecoder processing mode

	SRSBool					DCEnable;    //DialogClariy Enable
	SRSBool					DefEnable;	 //Definition Enable
	SRSInt16				DCLevel;	 //DialogClarity Level
	SRSInt16				DefLevel;	 //Definition Level

	SRSCshpTruBassControls	TBHDControls;	//TruBass HD controls

	SRSBool					LmtEnable;		//Limiter Enable
	SRSInt32				LmtMinimalGain; //Limiter minimal gain (headroom)
} SRSCshpControls;

#endif //__SRS_CSHP_DEF_H__
