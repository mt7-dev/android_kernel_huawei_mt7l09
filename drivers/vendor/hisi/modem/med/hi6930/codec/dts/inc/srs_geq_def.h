/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SRS GraphicEQ types, constants
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_graphiceq/std_fxp/include/srs_geq_def.h#3 $
 *  $Author: oscarh $
 *  $Date: 2010/10/21 $
 *	
********************************************************************************/

#ifndef __SRS_GRAPHICEQ_DEF_H__
#define __SRS_GRAPHICEQ_DEF_H__

#include "srs_typedefs.h"
#include "srs_fxp.h"
#include "srs_limiter_def.h"


//Integer word length of band gains:
#define SRS_GEQ_BAND_GAIN_IWL		4

//dB constants for GEQ band gains:
#define SRS_GEQ_MINUS_15DB			SRS_FXP16(0.177, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_14DB			SRS_FXP16(0.20, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_13DB			SRS_FXP16(0.224, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_12DB			SRS_FXP16(0.251, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_11DB			SRS_FXP16(0.282, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_10DB			SRS_FXP16(0.316, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_09DB			SRS_FXP16(0.355, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_08DB			SRS_FXP16(0.40, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_07DB			SRS_FXP16(0.45, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_06DB			SRS_FXP16(0.50, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_05DB			SRS_FXP16(0.56, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_04DB			SRS_FXP16(0.63, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_03DB			SRS_FXP16(0.71, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_02DB			SRS_FXP16(0.79, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_MINUS_01DB			SRS_FXP16(0.89, SRS_GEQ_BAND_GAIN_IWL)

#define SRS_GEQ_0DB					SRS_FXP16(1.00, SRS_GEQ_BAND_GAIN_IWL)

#define SRS_GEQ_PLUS_01DB			SRS_FXP16(1.12, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_02DB			SRS_FXP16(1.26, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_03DB			SRS_FXP16(1.41, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_04DB			SRS_FXP16(1.58, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_05DB			SRS_FXP16(1.78, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_06DB			SRS_FXP16(2.00, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_07DB			SRS_FXP16(2.24, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_08DB			SRS_FXP16(2.51, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_09DB			SRS_FXP16(2.82, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_10DB			SRS_FXP16(3.16, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_11DB			SRS_FXP16(3.55, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_12DB			SRS_FXP16(4.00, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_13DB			SRS_FXP16(4.47, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_14DB			SRS_FXP16(5.01, SRS_GEQ_BAND_GAIN_IWL)
#define SRS_GEQ_PLUS_15DB			SRS_FXP16(5.63, SRS_GEQ_BAND_GAIN_IWL)


//5-band:
#define SRS_GEQ_5BAND_NUM_OF_BANDS		5

typedef enum
{
	SRS_GEQ_5BAND_100HZ,
	SRS_GEQ_5BAND_300HZ,
	SRS_GEQ_5BAND_1000HZ,
	SRS_GEQ_5BAND_3000HZ,
	SRS_GEQ_5BAND_10000HZ,
	SRS_GEQ_5BAND_UNUSED = 0x7FFFFFFF
} SRSGeq5BandCenterFreq;

//Constants for maximum and minimum gain in each band:
#define SRS_5BAND_GEQ_BAND_GAIN_MAX				SRS_GEQ_PLUS_15DB
#define SRS_5BAND_GEQ_BAND_GAIN_MIN				SRS_GEQ_MINUS_15DB

typedef enum
{
	SRS_5BAND_GEQ_EXTRA_BAND_BEHAVIOR_DEFAULT,
	SRS_5BAND_GEQ_EXTRA_BAND_BEHAVIOR_UNITY,
	SRS_5BAND_GEQ_EXTRA_BAND_BEHAVIOR_SHELF,
	SRS_5BAND_GEQ_EXTRA_BAND_BEHAVIOR_UNUSED = 0x7FFFFFFF
} SRS5BandGeqExtraBandBehavior;


typedef struct _SRSGeq5bObj	*SRSGeq5bObj;

#define SRS_5BAND_GRAPHICEQ_OBJ_SIZE	(sizeof(_SRSGeq5bObj_t)+8+SRS_LIMITER_OBJ_SIZE)
#define	SRS_5BAND_GRAPHICEQ_WORKSPACE_SIZE(blockSize)	(sizeof(SRSInt32)*(blockSize)*2+8)

//10-band:
#define SRS_GEQ_10BAND_NUM_OF_BANDS		10

typedef enum
{
	SRS_GEQ_10BAND_31HZ,
	SRS_GEQ_10BAND_62HZ,
	SRS_GEQ_10BAND_125HZ,
	SRS_GEQ_10BAND_250HZ,
	SRS_GEQ_10BAND_500HZ,
	SRS_GEQ_10BAND_1000HZ,
	SRS_GEQ_10BAND_2000HZ,
	SRS_GEQ_10BAND_4000HZ,
	SRS_GEQ_10BAND_8000HZ,
	SRS_GEQ_10BAND_16000HZ,
	SRS_GEQ_10BAND_UNUSED = 0x7FFFFFFF
} SRSGeq10BandCenterFreq;

//Constants for maximum and minimum gain in each band:
#define SRS_10BAND_GEQ_BAND_GAIN_MAX				SRS_GEQ_PLUS_15DB
#define SRS_10BAND_GEQ_BAND_GAIN_MIN				SRS_GEQ_MINUS_15DB

typedef struct _SRSGeq10bObj	*SRSGeq10bObj;

#define SRS_10BAND_GRAPHICEQ_OBJ_SIZE	(sizeof(_SRSGeq10bObj_t)+8+SRS_LIMITER_OBJ_SIZE)
#define	SRS_10BAND_GRAPHICEQ_WORKSPACE_SIZE(blockSize)	(sizeof(SRSInt32)*(blockSize)+8)

/////////////////////////////////////////////////////////////////////////////////////////////////////////

	
#endif /*__SRS_GRAPHICEQ_DEF_H__*/
