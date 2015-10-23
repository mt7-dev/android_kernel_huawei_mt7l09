/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  srs_widesurround typedefs, constants
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/

#ifndef __SRS_WIDESURROUND_DEF_H__
#define __SRS_WIDESURROUND_DEF_H__

#include "srs_typedefs.h"
#include "srs_iir_def.h"
#include "srs_fft_def.h"

/*Data type definition here:*/
typedef  struct _SRSWideSrObj{int _;} *SRSWideSrdObj;

#define SRS_WIDESRD_WORKSPACE_SIZE		(sizeof(SRSInt32)*SRS_WIDESRD_BLK_SZ*2+8)
#define	SRS_WIDESRD_BLK_SZ		64

#define SRS_WIDESRD_GAIN_IWL		1
#define SRS_WIDESRD_SPK_SEP_IWL	1

typedef struct
{
	SRSBool				Enable;				//enable: SRS_True
	SRSInt16			InputGain;			//input gain, 0~1, default 0.5, I16.SRS_WIDESRD_GAIN_IWL
	SRSInt16			BypassGain;			//bypass gain, 0~1, default 1.0, I16.SRS_WIDESRD_GAIN_IWL
	SRSInt16			SpeakerSeparationLevel;	//Speaker separation, 0~1, default 0.272, I16.SRS_WIDESRD_SPK_SEP_IWL
	SRSInt16			CenterBoostGain;	//Center boost gain, 0.25~1, default 0.625, I16.SRS_WIDESRD_GAIN_IWL
	SRSBool				HandsetHPFEnable;	//Enable/disable the handset device HPF
} SRSWideSrdControls;




#endif /*__SRS_WIDESURROUND_DEF_H__*/
