/*
	SRS Labs CONFIDENTIAL
	@Copyright 2009 by SRS Labs.
	All rights reserved.

	(RCS keywords below, do not edit)
	$Id: //srstech/srs_common/std_fxp/include/srs_typedefs.h#9 $
  $Author: weiguoy $
  $Date: 2010/10/26 $
	
	Defines various basic data types
	
	This file should not contain anything but #defines and no // comments.
	This is because it is used to preprocess many kinds of files.
*/

#ifndef __SRS_TYPEDEFS_H__
#define __SRS_TYPEDEFS_H__

#include "srs_platdefs.h"

/******************************************************************************
 *Important Note:
 *   Read on if you are porting SRS IPs to your platform
 * Carefully check the integer type definitions below to see if they hold for
 * your platform. If not, redefine them for your platform
 *****************************************************************************/


#if defined(_WIN64) || __x86_64 || __amd64 //for storing pointers in integers (32-bit or 64-bit)
	typedef long long	SRSIntPtr;		//64-bit systems
#else
	typedef int			SRSIntPtr;		//32-bit systems
#endif

typedef int		SRSBool;
typedef  float	SRSFloat32;
#define SRS_False	0
#define SRS_True	1

typedef char	SRSInt8;
typedef short	SRSInt16;
typedef int		SRSInt32;

typedef unsigned char 		SRSUint8;
typedef unsigned short 		SRSUint16;
typedef unsigned int			SRSUint32;

typedef int							SRSInt24;
typedef unsigned int		SRSUint24;


#if SRS_TIC6X	/*long is 40-bit in TI compiler CCS for C6000 DSP's*/
typedef long 					SRSInt40;
typedef unsigned long SRSUint40;
#endif

typedef long long SRSInt64;
typedef unsigned long long SRSUint64;

/*Stereo channel*/
typedef struct
{
	SRSInt32 *Left;
	SRSInt32 *Right;
} SRSStereoCh;

typedef struct
{
	SRSInt32 *Left;
	SRSInt32 *Right;
	SRSInt32 *Center;
	SRSInt32 *Sub;
	SRSInt32 *SLeft;
	SRSInt32 *SRight;
	SRSInt32 *BLeft;
	SRSInt32 *BRight;
} SRS7Point1Ch;//SRS7_1Ch;

typedef struct
{
	SRSInt32 *Left;
	SRSInt32 *Right;
	SRSInt32 *Center;
	SRSInt32 *Sub;
	SRSInt32 *SLeft;
	SRSInt32 *SRight;
	SRSInt32 *SCenter;
} SRS6Point1Ch;//SRS6_1Ch;

typedef struct
{
	SRSInt32 *Left;
	SRSInt32 *Right;
	SRSInt32 *Center;
	SRSInt32 *Sub;
	SRSInt32 *SLeft;
	SRSInt32 *SRight;
} SRS5Point1Ch;//SRS5_1Ch;

typedef enum
{
	SRS_STAT_NO_ERROR = 0,			//Status: no error
	SRS_ERR_UNINITIALIZED = -1001,	//All error codes must be <0
	SRS_ERR_INVALID_PARAMETER,
	SRS_ERR_NOT_SUPPORTED,
	SRS_ERR_INCOMPATIBLE,
	SRS_ERR_ALLOC_FAILED,
	SRS_ERR_FFT_UNINITIALIZED,
	SRS_STAT_UNUSED = 0x7FFFFFFF
} SRSResult;

typedef enum
{
	SRS_MEMTYPE_PERSISTENT,	//Generic persistent memory
	SRS_MEMTYPE_SCRATCH,	//Generic scratch memory
	SRS_MEMTYPE_UNUSED = 0x7FFFFFFF
} SRSMemType;

/*
SRS_Alloc
Purpose:
This callback is used by the creation function to assign memory for the SRS technology object.
Arguments:
	eType ¨C The type of the memory to be allocated/assigned
	lenBytes ¨C Minimum number of bytes required
Return Value:
	Address of memory assigned
*/ 
typedef void* (*SRS_Allocator)(int lenBytes, SRSMemType eType, void* pContext);


typedef enum
{
	SRSFilter16,		//PEQ filters coefficients are in 16-bit fixed-point
	SRSFilter32,		//PEQ filters coefficients are in 32-bit fixed-point
	SRSFilter_UNUSED = 0x7FFFFFFF
} SRSFilterWl;	//filter word length


//SRSIOMode: descripting the I/O channels of SRS modules:
#define SRSIOLeftChBit					(1)
#define SRSIORightChBit					(1 << 1)
#define SRSIOCenterChBit				(1 << 2)
#define SRSIOSubChBit					(1 << 3)
#define SRSIOSLeftChBit					(1 << 4)
#define SRSIOSRightChBit				(1 << 5)
#define SRSIOSCenterChBit				(1 << 6)
#define SRSIOLtRtBit					(1 << 8)
#define SRSIOHeadphoneBit				(1 << 9)
#define SRSIOBSDigitalBit				(1 << 10)
#define SRSIOPL2MusicBit				(1 << 11)
//#define SRSIOCSIIBit					(1 << 12)

typedef enum
{
	SRS_IO_1_0_0 = SRSIOLeftChBit,	//a.k.a SRS_IO_MONO, mono input/output
	SRS_IO_2_0_0 = SRSIOLeftChBit | SRSIORightChBit,	//a.k.a SRS_IO_STEREO, stereo input/output
	SRS_IO_3_0_0 = SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit,
	SRS_IO_2_0_1 = SRSIOLeftChBit | SRSIORightChBit | SRSIOSubChBit,
	SRS_IO_3_0_1 = SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit | SRSIOSubChBit,
	SRS_IO_2_2_0 = SRSIOLeftChBit | SRSIORightChBit | SRSIOSLeftChBit | SRSIOSRightChBit,
	SRS_IO_3_2_0 = SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit | SRSIOSLeftChBit | SRSIOSRightChBit,
	SRS_IO_2_2_1 = SRSIOLeftChBit | SRSIORightChBit | SRSIOSubChBit | SRSIOSLeftChBit | SRSIOSRightChBit,
	SRS_IO_3_2_1 = SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit | SRSIOSubChBit | SRSIOSLeftChBit | SRSIOSRightChBit,
	SRS_IO_2_1_0 = SRSIOLeftChBit | SRSIORightChBit | SRSIOSCenterChBit,
	SRS_IO_3_1_0 = SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit | SRSIOSCenterChBit,
	SRS_IO_2_1_1 = SRSIOLeftChBit | SRSIORightChBit | SRSIOSubChBit | SRSIOSCenterChBit,
	SRS_IO_3_1_1 = SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit | SRSIOSubChBit | SRSIOSCenterChBit,
	SRS_IO_2_3_0 = SRSIOLeftChBit | SRSIORightChBit | SRSIOSLeftChBit | SRSIOSRightChBit | SRSIOSCenterChBit,
	SRS_IO_3_3_0 = SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit | SRSIOSLeftChBit | SRSIOSRightChBit | SRSIOSCenterChBit,
	SRS_IO_2_3_1 = SRSIOLeftChBit | SRSIORightChBit | SRSIOSubChBit | SRSIOSLeftChBit | SRSIOSRightChBit | SRSIOSCenterChBit,
	SRS_IO_3_3_1 = SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit | SRSIOSubChBit | SRSIOSLeftChBit | SRSIOSRightChBit | SRSIOSCenterChBit,
	SRS_IO_LtRt = SRSIOLeftChBit | SRSIORightChBit | SRSIOLtRtBit,
	SRS_IO_Headphone = SRSIOLeftChBit | SRSIORightChBit | SRSIOHeadphoneBit,
	SRS_IO_BSDigital = SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit | SRSIOSubChBit | SRSIOSLeftChBit | SRSIOSRightChBit | SRSIOBSDigitalBit,	//woth subwoofer or not?
	SRS_IO_PL2Music =  SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit | SRSIOSubChBit | SRSIOSLeftChBit | SRSIOSRightChBit | SRSIOPL2MusicBit,	//with subwoofer or not?
	//	SRS_IO_CSII = SRSIOLeftChBit | SRSIORightChBit | SRSIOCenterChBit | SRSIOSLeftChBit | SRSIOSRightChBit | SRSIOCSIIBit,	//with new cs decoder IP, the subwoofer output is mixed into SLeft and SRight
	SRS_IO_UNUSED = 0x7FFFFFFF
} SRSIOMode;

#define SRS_IO_MONO		SRS_IO_1_0_0
#define SRS_IO_STEREO	SRS_IO_2_0_0

/*
 *	Technology Version Number
 *
 *	kSrsVersionMajor
 *
 *	The major version number of a technology will change if
 *	backward compatibility with previous versions is broken.
 *
 *	kSrsVersionMinor
 *
 *	The minor version is incremented upon each new release of the
 *	software that includes new features, but maintains backward
 *	compatibility with the previous release.
 *
 *	kSrsVersionRevision
 *
 *	The revision is incremented upon each new release that includes
 *	minor modifications or fixes, but no additional new features.
 *	Revision increments also maintain backward compatibility
 *	with the previous release.
 *
 *	kSrsVersionRelease
 *
 *	The release number is either zero or 255 for standard releases along
 *	the main development path.  The value 255 should be considered to be
 *	equivalent to the value zero.  Other number ranges for the release have
 *	the following meanings:
 *
 *		  1 -  99	Reserved for internal use by SRS Labs, Inc.
 *		100 - 199	Alpha release
 *		200 - 254	Beta release
 *
 *	Depending on the purpose of the release, nonzero release numbers may or
 *	may not maintain compatibility with the corresponding standard release.
 *
 */
typedef enum
{
	SRS_VERSION_MAJOR,
    SRS_VERSION_MINOR,
    SRS_VERSION_REVISION,
    SRS_VERSION_RELEASE,
	SRS_VERSION_UNUSED = 0x7FFFFFFF
} SRSVersion;

typedef unsigned char (*SRSGetVersionProc)(SRSVersion which);
typedef struct
{
	const char					*LibName;		//The library name
	const SRSGetVersionProc		GetLibVerProc;	//The procedure to query the library version
} SRSLibInfo;	//The structure containing information about the library

#endif /*__SRS_TYPEDEFS_H__*/
