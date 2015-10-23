/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  The header file defines SRS fixed-point related data types and macros
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id: //srstech/srs_common/std_fxp/include/srs_fxp.h#2 $
 *  $Author: oscarh $
 *  $Date: 2010/06/02 $
********************************************************************************/

#ifndef __SRS_FXP_H__
#define __SRS_FXP_H__

#include "srs_typedefs.h"

typedef enum
{
	SRS_RND_INF,				// Rounding to infinity, (a.k.a round to nearest)
	SRS_TRN_ZERO,				// Truncation to zero (a.k.a magnitude truncation, floating to integer conversion in C uses this mode)
	SRS_QUANTIZATION_UNUSED = 0x7FFFFFFF
} SRSQuantizationMode;

#define _SIGNED_HALF(val)	((val)>=0? 0.5:-0.5)

#define _MININTVAL(wl)		((SRSInt64)~0<<((wl)-1))		//min value of the integer with word length wl
#define _MAXINTVAL(wl)		(~_MININTVAL(wl))				//max value of the integer with word length wl

#define _CLIP(val, wl)		((val)> _MAXINTVAL(wl)? _MAXINTVAL(wl):((val)<_MININTVAL(wl)? _MININTVAL(wl):val))

#define _FLOAT_VAL_OF_FXP_REP(val, wl, iwl)		((val)*((SRSInt64)1<<((wl)-(iwl))))

#define SRS_FXP16(val, iwl)		((SRSInt16)_CLIP(_FLOAT_VAL_OF_FXP_REP(val, 16, iwl) + _SIGNED_HALF(val), 16))		//convert to 16-bit fxp with SRS_RND_INF mode
#define SRS_FXP32(val, iwl)		((SRSInt32)_CLIP(_FLOAT_VAL_OF_FXP_REP(val, 32, iwl) + _SIGNED_HALF(val), 32))		//convert to 32-bit fxp with SRS_RND_INF mode
#define	SRS_FXP24(val, iwl)		((SRSInt24)_CLIP(_FLOAT_VAL_OF_FXP_REP(val, 24, iwl) + _SIGNED_HALF(val), 24))			//convert to 24-bit fxp with SRS_RND_INF mode

#endif //__SRS_FXP_H__
