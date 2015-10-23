/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SRS SRS3D types, constants
 *
 *	Author: Zesen Zhuang
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_srs3d/std_fxp/include/srs_srs3d_def.h#3 $
 *  $Author: oscarh $
 *  $Date: 2011/02/14 $
 *	
********************************************************************************/

#ifndef __SRS_SRS3D_DEF_H__
#define __SRS_SRS3D_DEF_H__

#include "srs_typedefs.h"


#define SRS_SRS3D_GAIN_IWL			1
#define SRS_SRS3D_CTRL_IWL			1
//#define SRS_SRS3D_INOUT_GAIN_IWL	1
typedef struct _SRSSrs3DObj *SRSSrs3DObj;

#define SRS_SRS3D_WORKSPACE_SIZE(blkLen)	(sizeof(SRSInt32)*(blkLen)*4+8)

/****** SRS 3D Modes ******/
typedef enum
{
	SRS_SRS3D_MONO,
	SRS_SRS3D_SINGLE_SPEAKER,
	SRS_SRS3D_STEREO,
	SRS_SRS3D_MODENUM,
	SRS_SRS3D_UNUSED = 0x7FFFFFFF
} SRSSrs3DMode;

#endif //__SRS_SRS3D_DEF_H__
