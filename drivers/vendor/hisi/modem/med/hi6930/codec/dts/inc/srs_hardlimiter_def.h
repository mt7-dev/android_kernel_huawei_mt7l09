/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  SRS HardLimiter types, constants
 *
 *	Author: Zesen Zhuang
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_hardlimiter/std_fxp/include/srs_hardlimiter_def.h#2 $
 *  $Author: jianc $
 *  $Date: 2010/11/25 $
 *	
********************************************************************************/

#ifndef __SRS_HARDLIMITER_DEF_H__
#define __SRS_HARDLIMITER_DEF_H__

#include "srs_typedefs.h"

typedef struct _SRSHdLmtObj *SRSHdLmtObj;

#define SRS_HDLMT_WORKSPACE_SIZE(blksize)		(sizeof(SRSInt32)*(blksize)*3+sizeof(SRSInt32)*SRS_HDLMT_MAX_DELAY_LEN+8)
#define	SRS_HDLMT_MAX_DELAY_LEN				48

#define		SRS_HDLMT_BOOST_IWL			6			//9
#define		SRS_HDLMT_INOUT_GAIN_IWL	3
#define		SRS_HDLMT_BYPASS_GAIN_IWL	1
#define		SRS_HDLMT_LIMIT_IWL			1




#endif //__SRS_HARDLIMITER_DEF_H__
