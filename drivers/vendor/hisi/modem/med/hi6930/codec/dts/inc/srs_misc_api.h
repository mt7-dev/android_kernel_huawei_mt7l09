/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  Header file for miscellaneous functions & macros
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id: //srstech/srs_common/std_fxp/include/srs_misc_api.h#13 $
 *  $Author: zesenz $
 *  $Date: 2011/02/22 $
********************************************************************************/

#ifndef __SRS_MISC_API_H
#define __SRS_MISC_API_H

#include "srs_typedefs.h"
 

#define SRS_ALIGNON8(p)		((void*)(((SRSIntPtr)(p)+7) & ((SRSIntPtr)(-1)<<3)))	//adjust pointer p to be aligned on 8

#ifdef __cplusplus 
	#define SRS_EXTERNC extern "C"
	#define SRS_INLINE	inline
#else
	#define SRS_EXTERNC
	#define SRS_INLINE
#endif /*__cplusplus*/

//Macro to define library version string and version get function:
//Usage example: SRS_DEFINE_LIB_VERSION(TruBass, 2, 1, 0, 100)
#define SRS_DEFINE_LIB_VERSION(tech, major, minor, rev, release)	SRS_DEFINE_LIB_VERSION_CORE(tech, major, minor, rev, release)
#define SRS_DEFINE_LIB_VERSION_CORE(tech, major, minor, rev, release) \
	SRS_EXTERNC const unsigned char SRS_##tech##_Lib_Ver_##major##_##minor##_##rev##_##release[] =  {major, minor, rev, release}; \
	SRS_EXTERNC unsigned char SRS_##tech##_GetLibVersion(SRSVersion which) \
	{ \
		return SRS_##tech##_Lib_Ver_##major##_##minor##_##rev##_##release[which]; \
	}

//Macro to define technology version string and version get function:
//Usage example: SRS_DEFINE_TECH_VERSION(TruBass, 2, 1, 0, 100)
#define SRS_DEFINE_TECH_VERSION(tech, major, minor, rev, release)		SRS_DEFINE_TECH_VERSION_CORE(tech, major, minor, rev, release)
#define SRS_DEFINE_TECH_VERSION_CORE(tech, major, minor, rev, release) \
	SRS_EXTERNC const unsigned char SRS_##tech##_Tech_Ver_##major##_##minor##_##rev##_##release[] =  {major, minor, rev, release}; \
	SRS_EXTERNC unsigned char SRS_##tech##_GetTechVersion(SRSVersion which) \
	{ \
		return SRS_##tech##_Tech_Ver_##major##_##minor##_##rev##_##release[which]; \
	}

//Macro to check version compatibility of a component:
#define SRS_VERSION_COMPATIBLE(GetVerFunc, major, minor) \
	(GetVerFunc(SRS_VERSION_MAJOR)==(major) && ((minor)?(GetVerFunc(SRS_VERSION_MINOR)>=(minor)):1))

//Macro to computes structure member address offset:
//structure: the name of the structure
//member: the name of the member whose offset is to be computed.
//e.g: to get the offset of OutputGain in structure _SRSTruBassSettings_t:
//#define OUTPUT_GAIN_OFFSET  SRS_STRUCT_MEM_OFFSET(_SRSTruBassSettings_t, OutputGain)
#define SRS_STRUCT_MEM_OFFSET(structure, member)	((int)(&((structure*)0)->member))

#define SRS_MAX(a, b)  ((a)>=(b)?(a):(b))
#define SRS_MIN(a, b)  ((a)<=(b)?(a):(b))

/* left shift with saturation, that is, Saturate(x<<iwl), x's type is SRSInt32  */
#define SRS_SAT_LEFTSHIFT(x, iwl)	( (((x)<<(iwl)) >> (iwl)) != (x) ? ((x)>>31) ^ 0x7FFFFFFF : (x)<<(iwl) )

/* Note: whether the macro should be kept will be discussed later, now the macro is defined to facilitate zesen's code clean up job */
#define MUL64TO32(a, b)		((SRSInt32)( ((SRSInt64)(a) * (SRSInt64)(b) + 0x40000000) >>31 ))

//calculate the absolute value of x
SRS_INLINE SRSInt32 SRS_Abs(SRSInt32 x)
{
	SRSInt32 out;

	out = (x) - ((x)<0);
	out = (out) ^ ((out)>>31);

	return out;
}

//saturation result of a+b
SRS_INLINE SRSInt32 SRS_SatAdd(SRSInt32 a, SRSInt32 b)
{
	SRSInt32 out;

	out = a+b;
	if(!( (a>>31)^(b>>31) )){
		if((out>>31)^(a>>31)){
			out = (a>>31) ^ 0x7FFFFFFF;
		}
	}

	return out;
}

//saturation result of a-b
SRS_INLINE SRSInt32 SRS_SatSub(SRSInt32 a, SRSInt32 b)
{
	SRSInt32 out;

	out = a-b;
	if((a>>31)^(b>>31)){
		if((out>>31)^(a>>31)){
			out = (a>>31) ^ 0x7FFFFFFF;
		}
	}

	return out;
}

/* Calculate Saturate( ((SRSInt64)a * (SRSInt64)b) >> (32-iwl) )
 * if a in I32.1, b in I32.iwl, then return value in I32.1
 */
SRS_INLINE SRSInt32 SRS_Mul64To32(SRSInt32 a, SRSInt32 b, int iwl)
{
	SRSInt64 acc;
	SRSInt32 hi32, acctmp;
	SRSUint32 lo32;

	acc = (SRSInt64)(a) * (SRSInt64)(b);
	acc += (0x01LL<<(32-iwl-1));
	hi32 = (SRSInt32)(acc>>32);
	lo32 = (SRSUint32)acc;

	acctmp = (hi32<<iwl);
	lo32 = (SRSUint32)(((SRSUint64)lo32)>>(32-iwl));
	acctmp = acctmp | lo32;
	if(((hi32<<iwl)>>iwl) != hi32){							//overflow
		acctmp = 0x7FFFFFFF ^ (hi32>>31);
	}
		
	return acctmp;
}

/*
 * A inline function version of macro SRS_FXP32X16, after replacing all SRS_FXP32X16
 * with this inline function, SRS_FXP32X16 will be removed
 * Calculate a*b, return high 32 bits
 */
SRS_INLINE SRSInt32 SRS_Mul32x16(SRSInt32 a, SRSInt16 b)
{
	SRSInt32 acc;

	acc = (SRSInt32)(SRSUint16)(a) * (SRSInt32)(b);
	acc += 0x8000;
	acc >>= 16;
	acc += ((a)>>16) * (SRSInt32)(b);

	return acc;
}

//32x32, return high 32 bits
SRS_INLINE SRSInt32 SRS_Mul32x32(SRSInt32 x, SRSInt32 y)
{
	SRSInt64 tmp;

	tmp = (SRSInt64)x * (SRSInt64)y;

	return (SRSInt32)(tmp>>32);
}

//32x32, return high 32 bits with rounding
SRS_INLINE SRSInt32  SRS_Mul32x32R(SRSInt32 x, SRSInt32 y)
{
	SRSInt64 acc;
	SRSInt32 lo32, hi32;

	acc = (SRSInt64)x * (SRSInt64)y;

	hi32 = (SRSInt32)(acc>>32);
	lo32 = (SRSInt32)acc;

	return hi32+(lo32<0? 1:0);

}

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

void SRS_CopyInt32Vector(SRSInt32 *src, SRSInt32 *dest, int n);
void SRS_MemSet(void *mem, int size, char val); 

#ifdef __cplusplus
}
#endif /*__cplusplus*/


#endif //__SRS_MISC_API_H
