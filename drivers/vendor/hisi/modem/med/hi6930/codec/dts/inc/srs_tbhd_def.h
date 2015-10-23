/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SRS Split Analysis TruBass types, constants
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_sa_trubass/std_fxp/include/srs_sa_trubass_def.h#2 $
 *  $Author: oscarh $
 *  $Date: 2011/02/15 $
 *	
********************************************************************************/

#ifndef __SRS_SA_TRUBASS_DEF_H__
#define __SRS_SA_TRUBASS_DEF_H__

#include "srs_typedefs.h"



typedef struct _SRSTBHDObj *SRSTBHDObj;


#define SRS_TBHD_WORKSPACE_SIZE(blockLen)	(5*blockLen*sizeof(SRSInt32)+8)	//in bytes

#define SRS_TBHD_GAIN_IWL		1	//iwl of input gain, output gain and bypass gain.
#define SRS_TBHD_CTRL_IWL		1	//iwl of Trubass Control
#define SRS_TBHD_REFLEVEL_IWL	4	//iwl of Reference Level control


typedef enum
{
	SRS_TBHD_SPEAKER_LF_RESPONSE_40HZ,
	SRS_TBHD_SPEAKER_LF_RESPONSE_60HZ,
	SRS_TBHD_SPEAKER_LF_RESPONSE_100HZ,
	SRS_TBHD_SPEAKER_LF_RESPONSE_120HZ,
	SRS_TBHD_SPEAKER_LF_RESPONSE_150HZ,
	SRS_TBHD_SPEAKER_LF_RESPONSE_200HZ,
	SRS_TBHD_SPEAKER_LF_RESPONSE_250HZ,
	SRS_TBHD_SPEAKER_LF_RESPONSE_300HZ,
	SRS_TBHD_SPEAKER_LF_RESPONSE_400HZ,
	SRS_TBHD_SPEAKER_LF_RESPONSE_CUSTOM,
	SRS_TBHD_SPEAKER_LF_RESPONSE_SIZES,
	SRS_TBHD_SPEAKER_LF_RESPONSE_UNUSED = 0x7FFFFFFF
} SRSTBHDSpeakerLFResponse;

//Properties describing speakers:
typedef struct
{
	SRSTBHDSpeakerLFResponse		AudioFilter;	//SRSTBHDSpeakerLFResponse, Frequency response of the audio filter
	SRSTBHDSpeakerLFResponse		AnalysisFilter;	//SRSTBHDSpeakerLFResponse, Frequency response of the analysis filter
} SRSTBHDSpeakerSize;


typedef union
{
	struct
	{
		SRSInt32	LowPassAudioFilterCoef[2];
		SRSInt32	LowBandAudioFilterCoef[3];
		SRSInt32	MidBandAudioFilterCoef[3];
		SRSInt32	LowPassAnalysisFilterCoef[2];
		SRSInt32	LowBandAnalysisFilterCoef[3];
		SRSInt32	MidBandAnalysisFilterCoef[3];
	} Struct;
	SRSInt32	Array[16];
} SRSTBHDCustomSpeakerCoefs;

typedef enum
{
	SRS_TBHD_MODE_MONO,
	SRS_TBHD_MODE_STEREO,
	SRS_TBHD_MODE_NUM,
	SRS_TBHD_MODE_UNUSED = 0x7FFFFFFF
} SRSTBHDMode;

typedef enum
{
	SRS_TBHD_OUTPUT_MIXED,		//Output the left channel and the right channel mixed with the bass component.
	SRS_TBHD_OUTPUT_SEPARATED,	//Output the left channel, the right channel and the bass component separately.
	SRS_TBHD_OUTPUT_UNUSED = 0x7FFFFFFF
} SRSTBHDOutputOption;

typedef struct
{
	SRSBool			Enable;
	SRSInt16		InputGain;
	SRSInt16		OutputGain;
	SRSInt16		BypassGain;

	SRSBool			LevelIndependentEnable;
	SRSBool			SplitAnalysisEnable;	//If False, AudioFilter is used for analysis, and the AnalysisFilter is not referenced.

	SRSInt16		Level;
	SRSInt16		CompressorLevel;
	SRSInt16		RefLevel;				//referenced only if LevelIndependentEnable is False

	SRSTBHDMode		Mode;

	SRSTBHDSpeakerSize	SpkSize; //{AudioFilter, AnalysisFilter}

} SRSTBHDControls; //All public control parameters of TruBass HD

#endif //__SRS_SA_TRUBASS_DEF_H__
