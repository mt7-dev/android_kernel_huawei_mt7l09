/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SRS HP360 types, constants
 *
 *	Author: Zesen Zhuang
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/

#ifndef __SRS_HP360_DEF_H__
#define __SRS_HP360_DEF_H__

#include "srs_typedefs.h"


typedef struct _SRSHp360Obj *SRSHp360Obj;


#define SRS_HP360_WORKSPACE_SIZE(blksize)	(sizeof(SRSInt32)*(blksize)*5+8)
#define SRS_HP360_GAIN_IWL			1

#define SRS_HP360_IN_GAIN_IWL			    1				//Iwl for HP360 Input Gain
#define SRS_HP360_OUT_GAIN_IWL			    1				//Iwl for HP360 Output Gain
#define SRS_HP360_BYPASS_GAIN_IWL		    1				//Iwl for HP360 Bypass Gain

#endif //__SRS_HP360_DEF_H__
