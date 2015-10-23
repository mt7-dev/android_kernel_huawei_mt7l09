/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  srs_wowhdplus typedefs, constants
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/

#ifndef __SRS_WOWHDPLUS_DEF_H__
#define __SRS_WOWHDPLUS_DEF_H__

#include "srs_typedefs.h"
#include "srs_iir_def.h"
#include "srs_focus_def.h"
#include "srs_def_def.h"
#include "srs_tbhd_def.h"
#include "srs_srs3d_def.h"
#include "srs_widesrd_def.h"



#define	 SRS_WOWHDX_GAIN_IWL				1

typedef  struct _SRSWowhdxObj{int _;} *SRSWowhdxObj;

#define SRS_WOWHDX_WORKSPACE_SIZE		(_SRS_WHDP_MAX(\
												_SRS_WHDP_MAX(sizeof(SRSInt32)*SRS_WOWHDX_BLK_SZ*2+8, SRS_TBHD_WORKSPACE_SIZE(SRS_WOWHDX_BLK_SZ)), \
												_SRS_WHDP_MAX(SRS_SRS3D_WORKSPACE_SIZE(SRS_WOWHDX_BLK_SZ), SRS_WIDESRD_WORKSPACE_SIZE) \
											 ) + sizeof(SRSInt32)*SRS_WOWHDX_BLK_SZ*2)

#define	SRS_WOWHDX_BLK_SZ				SRS_WIDESRD_BLK_SZ

#define SRS_WOWHDX_XOVER_ORDER	4		//The order of the cross over filter


typedef enum
{
	SRS_WOWHDX_STEREO_ENHANCE_NONE,		//No stereo enhancement
	SRS_WOWHDX_STEREO_ENHANCE_SRS3D,	//SRS3D mode
	SRS_WOWHDX_STEREO_ENHANCE_WDSRD,	//WideSurround mode
	SRS_WOWHDX_STEREO_ENHANCE_UNUSED = 0x7FFFFFFF  //force to generate 32-bit enum
} SRSWowhdxStereoEnhanceMode;

typedef struct
{
	//The order of the crossover is fixed to 4
	const SRSInt16		*XoverLpfCoefs;	//The coefficients of xover LPF
	const SRSInt16		*XoverHpfCoefs;	//The coefficients of xover HPF
	
	int					HpfOrder;		//The HPF order
	const SRSInt16		*HpfCoefs;		//The HPF coefficients

} SRSWowhdxFilterConfig;

typedef struct
{
	int						MaxHpfOrder;		//The maximum HPF order that the WOW HDX object should support
	
} SRSWowhdxCreateData;

typedef struct
{
	SRSBool				Enable;
	SRSBool				SplitAnalysisEnable;  //Enable/disable the split analysis feature
	SRSBool				LevelIndependentEnable; //Enable/disalbe the level independent feature
	SRSInt16			Level;	//TruBass Level, 0.0~1.0
	SRSInt16			CompressorLevel;	//0.0~1.0
	SRSTBHDSpeakerSize	SpkSize; //{AudioFilter, Analysis Filter}
	SRSTBHDMode			Mode;		//Mono mode or stereo mode TB
} SRSWowhdxTruBassControls;

typedef struct
{
	SRSInt16			SpaceLevel;
	SRSInt16			CenterLevel;
	SRSBool				HeadphoneEnable;
	SRSBool				HighBitRateEnable;
	SRSSrs3DMode		SRS3DMode;
} SRSWowhdxSRS3DControls;

typedef struct
{
	SRSInt16			SpeakerSeparationLevel;	//Speaker separation, 0~1, default 0.272
	SRSInt16			CenterBoostGain;	//Center boost gain, 0.25~1, default 0.625
	SRSBool				HandsetHPFEnable;	//Enable/disable the handset device HPF
} SRSWowhdxWideSrdControls;

typedef struct
{
	SRSBool						Enable;
	SRSInt16					InputGain;
	SRSInt16					OutputGain;
	SRSInt16					BypassGain;
	SRSWowhdxStereoEnhanceMode	EnhanceMode;
	SRSBool						XoverEnable;
	SRSBool						FocusEnable;
	SRSBool						DefEnable;
	SRSInt16					FocusLevel;
	SRSInt16					DefLevel;
	SRSBool						HpfEnable;

	SRSWowhdxTruBassControls	TBHDControls;
	SRSWowhdxSRS3DControls		SRS3DControls;
	SRSWowhdxWideSrdControls	WdSrdControls;
} SRSWowhdxControls;

//////////////////////////////////////////////////////////////////////////
#define _SRS_WHDP_MAX(a, b)	((a)>=(b)? (a):(b))

#endif /*__SRS_WOWHDPLUS_DEF_H__*/
