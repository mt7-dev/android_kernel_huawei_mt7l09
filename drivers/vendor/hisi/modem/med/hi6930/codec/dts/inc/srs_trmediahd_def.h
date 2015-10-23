/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  srs_trmediahd typedefs, constants
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/

#ifndef __SRS_TRMEDIAHD_DEF_H__
#define __SRS_TRMEDIAHD_DEF_H__

#include "srs_iir_def.h"
#include "srs_peq_def.h"
#include "srs_cshp_def.h"
#include "srs_hardlimiter_def.h"
#include "srs_geq_def.h"
#include "srs_wowhdx_def.h"


#define SRS_TRMEDIAHD_GAIN_IWL	1  //iwl of input gain, output gain, bypass gain and Headroom gain.

/*Data types and constants definition here:*/

typedef struct _SRSTrMediaHDObj	*SRSTrMediaHDObj;

#define SRS_TRMEDIAHD_WORKSPACE_SIZE(blkLen)	SRS_MAX(SRS_MAX(SRS_CSHP_WORKSPACE_SIZE(blkLen), SRS_WOWHDX_WORKSPACE_SIZE),\
														SRS_MAX(SRS_HDLMT_WORKSPACE_SIZE(blkLen), SRS_10BAND_GRAPHICEQ_WORKSPACE_SIZE(blkLen)))
														

typedef struct
{
	int		HpfOrder;	//The order of the HPF
	int		LPeqBands;	//The number of bands of the left channel TruEQ
	int		RPeqBands;	//The number of bands of the right channel TruEQ
}SRSTrMediaHDFilterOrders;

typedef struct{
	SRSTrMediaHDFilterOrders		MaxFilterOrders;	//The maximum filter orders to support
}SRSTrMediaHDCreateData;

typedef struct{
	SRSTrMediaHDFilterOrders		FilterOrders;
	//The order of the crossover is fixed to 4
	const SRSInt16				*XoverLpfCoefs;	//The coefficients of WOW HDX xover LPF, could be NULL if WOW HDX not used
	const SRSInt16				*XoverHpfCoefs;	//The coefficients of WOW HDX xover HPF  could be NULL if WOW HDX not used
	const SRSInt16				*HpfCoefs;
	const SRSInt32				*LeftTruEqCoefs;
	const SRSInt32				*RightTruEqCoefs;
} SRSTrMediaHDFilterConfig;

typedef enum
{
	SRS_TRMEDIAHD_MAINTECH_NONE, //bypass the MainTech block
	SRS_TRMEDIAHD_MAINTECH_WOWHDX,	//processing block length must be 64 to use WOW HDX
	SRS_TRMEDIAHD_MAINTECH_CSHP,
	SRS_TRMEDIAHD_MAINTECH_UNUSED = 0x7FFFFFFF  //force to generate 32-bit enum
} SRSTrMediaHDMainTech;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Control structures:
typedef struct
{
	SRSBool				Enable;
	SRSBool				SplitAnalysisEnable;  //Enable/disable the split analysis feature
	SRSInt16			CompressorLevel;	//0.0~1.0
	SRSInt16			Level;	//TruBass Level, 0.0~1.0
	SRSTBHDSpeakerSize	SpkSize; //{AudioFilter, Analysis Filter}
	SRSTBHDMode			Mode;		//Mono mode or stereo mode TB
} SRSTrMediaHDTBHDControls;

typedef struct
{
	SRSInt16			SpaceLevel;
	SRSInt16			CenterLevel;
	SRSBool				HeadphoneEnable;
	SRSBool				HighBitRateEnable;
	SRSSrs3DMode		Mode;
} SRSTrMediaHDSRS3DControls;

typedef struct
{
	SRSInt16			SpeakerSeparationLevel;	//Speaker separation, 0~1, 
	SRSInt16			CenterBoostGain;	//Center boost gain, 0.25~1
	SRSBool				HandsetHPFEnable;	//Enable/disable the handset device HPF
} SRSTrMediaHDWideSrdControls;

//CSHP controls to be exposed:
typedef struct
{
	SRSInt16					InputGain;
	SRSCshpRenderingMode		Render;		//Using CSDecoder or Passive Matrix Decoder
	SRSCSDProcMode				CSDProcMode; //CSDecoder processing mode

	SRSBool						DCEnable;    //DialogClariy Enable
	SRSBool						DefEnable;	 //Definition Enable
	SRSInt16					DCLevel;	 //DialogClarity Level
	SRSInt16					DefLevel;	 //Definition Level

	SRSTrMediaHDTBHDControls	TBHDControls;	//TruBass HD controls

	SRSBool						LmtEnable;		//Limiter Enable
	SRSInt32					LmtMinimalGain; //Limiter minimal gain
} SRSTrMediaHDCshpControls;


//WOW HDX controls to be exposed:
typedef struct
{
	SRSInt16					InputGain;
	SRSWowhdxStereoEnhanceMode	EnhanceMode;
	SRSBool						XoverEnable;
	SRSBool						FocusEnable;
	SRSBool						DefEnable;
	SRSInt16					FocusLevel;
	SRSInt16					DefLevel;

	SRSTrMediaHDTBHDControls	TBHDControls;
	SRSTrMediaHDSRS3DControls	SRS3DControls;
	SRSTrMediaHDWideSrdControls	WdSrdControls;
} SRSTrMediaHDWowhdxControls;

//Hardlimiter controls to be exposed:
typedef struct
{
	SRSBool				Enable;
	SRSInt16			OutputGain;
	SRSInt16			BypassGain;
	SRSInt32			BoostGain;
	int					DelayLen;
} SRSTrMediaHDHdLmtControls;	//HardLimiter's controls

//All public controls of TruMedia HD:
typedef struct
{
	SRSBool					Enable;
	SRSInt16				InputGain;
	SRSInt16				OutputGain;
	SRSInt16				BypassGain;
	SRSInt16				HeadrmGain;
	SRSTrMediaHDMainTech	MainTech;

	SRSBool					EnableGEQ;
	SRSBool					EnableHPF;
	SRSBool					EnableTruEQ;

	SRSTrMediaHDCshpControls	CshpControls;
	SRSTrMediaHDWowhdxControls	WowhdxControls;
	SRSTrMediaHDHdLmtControls	HdLmtControls;
} SRSTrMediaHDControls;

typedef struct
{
    int HPF1_order;
    int HPF2_order;
}SRSTrMediaHDHpfOrder;


#ifndef SRS_MAX
	#define SRS_MAX(a, b)  ((a)>=(b)?(a):(b))
#endif

#endif /*__SRS_TRMEDIAHD_DEF_H__*/
