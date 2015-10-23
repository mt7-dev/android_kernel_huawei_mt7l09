


#ifndef __OSL_TYPES_H
#define __OSL_TYPES_H

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#else
#if defined(__CMSIS_RTOS) || defined(__VXWORKS__) /* linux中有bool枚举定义 */
#ifndef bool
typedef enum {
	false	= 0,
	true	= 1
}bool;
#endif /* bool */
#endif

#endif /* __cplusplus */

#ifdef __KERNEL__
#include <linux/types.h>
#ifndef __ASSEMBLY__
#define UNUSED(a) (a=a)
#endif

#ifndef __inline__
#define __inline__ inline
#endif

#elif defined(_WRS_KERNEL) || defined(__VXWORKS__) ||  defined(VXWORKS)

#ifndef __ASSEMBLY__

#ifndef inline
#define inline __inline__
#endif

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#define S8_C(x)  x
#define U8_C(x)  x ## U
#define S16_C(x) x
#define U16_C(x) x ## U
#define S32_C(x) x
#define U32_C(x) x ## U
#define S64_C(x) x ## LL
#define U64_C(x) x ## ULL
#define UNUSED(a) (a=a)


#else /* __ASSEMBLY__ */

#define S8_C(x)  x
#define U8_C(x)  x
#define S16_C(x) x
#define U16_C(x) x
#define S32_C(x) x
#define U32_C(x) x
#define S64_C(x) x
#define U64_C(x) x

#endif /* __ASSEMBLY__ */

#ifndef __ASSEMBLY__

/* compiler.h */
/*
 * This is used to ensure the compiler did actually allocate the register we
 * asked it for some inline assembly sequences.  Apparently we can't trust
 * the compiler from one version to another so a bit of paranoia won't hurt.
 * This string is meant to be concatenated with the inline asm string and
 * will cause compilation to stop on mismatch.
 * (for details, see gcc PR 15089)
 */
#define __asmeq(x, y)  ".ifnc " x "," y " ; .err ; .endif\n\t"

/* stddef.h */
#undef NULL
#define NULL ((void *)0)

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((u32) &((TYPE *)0)->MEMBER)
#endif

#endif /* __ASSEMBLY__ */

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) 			\
	 ((type *)((char *)(ptr) - offsetof(type,member)))


#ifndef printk
#define printk  printf
#endif

#elif defined(__CMSIS_RTOS)

#ifndef __ASSEMBLY__

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((u32) &((TYPE *)0)->MEMBER)
#endif
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) 			\
	 ((type *)((char *)(ptr) - offsetof(type,member)))
typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

/* stddef.h */
#undef NULL
#define NULL ((void *)0)

#define UNUSED(a) (a=a)

#endif

#else

#ifndef __ASSEMBLY__
typedef unsigned int u32;
#endif

#endif /* __KERNEL__ */

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif

