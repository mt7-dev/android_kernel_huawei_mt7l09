/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SRS Definition types, constants...
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id$
 *  $Author$
 *  $Date$
********************************************************************************/
#ifndef __SRS_DEFINITION_DEF_H__
#define __SRS_DEFINITION_DEF_H__

#include "srs_typedefs.h"


typedef struct _SRSDefObj *SRSDefObj;

#define SRS_DEF_WORKSPACE_SIZE(blockSamples)	(sizeof(SRSInt32)*(blockSamples)+8)

#define SRS_DEF_IWL	1


#endif //__SRS_DEFINITION_DEF_H__
