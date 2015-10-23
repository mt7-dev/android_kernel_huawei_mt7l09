#ifndef  __OSL_MATH64_H__
#define  __OSL_MATH64_H__

#ifdef __KERNEL__
#include <linux/math64.h>
#elif defined(__VXWORKS__)||defined(__CMSIS_RTOS)
#ifndef div_u64_rem
static inline unsigned long long div_u64_rem(unsigned long long  dividend,  unsigned int  divisor,  unsigned int  *remainder)
{
	*remainder = dividend % divisor;
	return dividend/divisor;
}
#endif
/**
 * div_u64 - unsigned 64bit divide with 32bit divisor
 *
 * This is the most common 64bit divide and should be used if possible,
 * as many 32bit archs can optimize this variant better than a full 64bit
 * divide.
 */
#ifndef div_u64
static inline unsigned long long div_u64(unsigned long long dividend, unsigned int  divisor)
{
	u32 remainder;
	return div_u64_rem(dividend, divisor, &remainder);
}
#endif

#endif/*KERNEL*/
#endif/*__OSL_MATH64_H__*/

