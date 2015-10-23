/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SRS CSDecoder types, constants
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/

#ifndef __SRS_CSDECODER_DEF_H__
#define __SRS_CSDECODER_DEF_H__

#include "srs_typedefs.h"
#include "srs_monotostereo_def.h"



/* CS Decoder Process Mode definition */
typedef enum
{
	SRS_CSD_CINEMA	= 1,
	SRS_CSD_PRO		= 1 << 1,
	SRS_CSD_MUSIC	= 1 << 2,
	SRS_CSD_MONO	= 1 << 3,
	SRS_CSD_LCRS	= 1 << 4,
	SRS_CSDProc_UNUSED = 0x7FFFFFFF
} SRSCSDProcMode;

/* CS Decoder Output Mode definition */
typedef enum
{
	SRS_CSD_STEREO,
	SRS_CSD_MULTICHS,
	SRS_CSDOut_UNUSED = 0x7FFFFFFF
} SRSCSDOutMode;

#define SRS_CSD_GAIN_IWL	1	//InputGain iwl

typedef struct _SRSCSDObj *SRSCSDObj;

#define SRS_CSD_WORKSPACE_SIZE(blksize)	(sizeof(SRSInt32)*(blksize)*17 + 8)



#endif //__SRS_CSDECODER_DEF_H__
