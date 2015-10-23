/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SRS WOWHD types, constants
 *
 *	Author: Zesen Zhuang
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_wowhd/std_fxp/include/srs_wowhd_def.h#3 $
 *  $Author: zesenz $
 *  $Date: 2010/11/22 $
 *	
********************************************************************************/

#ifndef __SRS_WOWHD_DEF_H__
#define __SRS_WOWHD_DEF_H__

#include "srs_typedefs.h"
#include "srs_srs3d_def.h"
#include "srs_def_def.h"
#include "srs_focus_def.h"
#include "srs_limiter_def.h"
#include "srs_tbhd_def.h"


#define SRS_WOWHD_GAIN_IWL	1


typedef struct _SRSWowhdObj *SRSWowhdObj;

#define SRS_WOWHD_WORKSPACE_SIZE(blksize)	(sizeof(SRSInt32)*6*(blksize)+40)

//Control structures:
typedef struct
{
	SRSBool				Enable;
	SRSBool				SplitAnalysisEnable;  //Enable/disable the split analysis feature
	//SRSBool			LevelIndependentEnable; //Enable/disalbe the level independent feature
	SRSInt16			CompressorLevel;	//0.0~1.0
	SRSInt16			Level;	//TruBass Level, 0.0~1.0
	SRSTBHDSpeakerSize	SpkSize; //{AudioFilter, Analysis Filter}
	SRSTBHDMode			Mode;		//Mono mode or stereo mode TB
} SRSWowhdTBHDControls;

typedef struct
{
	SRSBool				Enable;
	SRSInt16			SpaceLevel;
	SRSInt16			CenterLevel;
	SRSBool				HeadphoneEnable;
	SRSBool				HighBitRateEnable;
	SRSSrs3DMode		Mode;
} SRSWowhdSRS3DControls;


typedef struct
{
	SRSBool						Enable;
	SRSInt16					InputGain;
	SRSInt16					OutputGain;
	SRSInt16					BypassGain;

	SRSBool						FocusEnable;
	SRSBool						DefEnable;
	SRSInt16					FocusLevel;
	SRSInt16					DefLevel;

	SRSWowhdTBHDControls		TBHDControls;
	SRSWowhdSRS3DControls		SRS3DControls;

	SRSBool						LmtEnable;		//Limiter enable
	SRSInt32					LmtMinGain;		//Limiter minimal gain
} SRSWowhdControls;



#endif //__SRS_WOWHD_DEF_H__
