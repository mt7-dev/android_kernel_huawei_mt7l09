/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  FOCUS types, constants
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id: //srstech/srs_common/std_fxp/include/srs_focus_def.h#2 $
 *  $Author: oscarh $
 *  $Date: 2011/02/14 $
********************************************************************************/
#ifndef __SRS_FOCUS_DEF_H__
#define __SRS_FOCUS_DEF_H__

#include "srs_typedefs.h"


typedef struct _SRSFocusObj *SRSFocusObj;

#define SRS_FOCUS_WORKSPACE_SIZE(blockSamples)	(sizeof(SRSInt32)*(blockSamples)+8)

#define SRS_FOCUS_IWL	1




#endif //__SRS_FOCUS_DEF_H__
