


#ifndef __BTRFS_MATH_H
#define __BTRFS_MATH_H

#include <asm/div64.h>

static inline u64 div_factor(u64 num, int factor)
{
	if (factor == 10)
		return num;
	num *= factor;
	do_div(num, 10);
	return num;
}

static inline u64 div_factor_fine(u64 num, int factor)
{
	if (factor == 100)
		return num;
	num *= factor;
	do_div(num, 100);
	return num;
}

#endif
