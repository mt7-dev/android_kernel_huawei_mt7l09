/* ------------------------------------------------------------------------ */
/*  IntegrIT, Ltd.   www.integrIT.ru,  info@integrIT.ru                     */
/*                                                                          */
/*  NatureDSP Signal Processing Library                                     */
/*                                                                          */
/* This library contains copyrighted materials, trade secrets and other     */
/* proprietary information of IntegrIT, Ltd. This software is licensed for  */
/* use with Tensilica HiFi2 core only and must not be used for any other    */
/* processors and platforms. The license to use these sources was given to  */
/* Tensilica, Inc. under Terms and Condition of a Software License Agreement*/
/* between Tensilica, Inc. and IntegrIT, Ltd. Any third party must not use  */
/* this code in any form unless such rights are given by Tensilica, Inc.    */
/* Compiling, linking, executing or using this library in any form you      */
/* unconditionally accept these terms.                                      */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2010 IntegrIT, Limited.                           */
/*                          All Rights Reserved.                            */
/* ------------------------------------------------------------------------ */
#ifndef __NATUREDSPTYPES_H__
#define __NATUREDSPTYPES_H__

#include <stddef.h>

#ifndef COMPILER_ANSI
// ----------------------------------------------------------
//             Compilers autodetection
// ----------------------------------------------------------
#define ___UNKNOWN_COMPILER_YET
#ifdef __ICC
  #define COMPILER_INTEL /* Intel C/C++ */
  #undef ___UNKNOWN_COMPILER_YET
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef _MSC_VER

#ifdef _ARM_
  #define COMPILER_CEARM9E /* Microsoft Visual C++,ARM9E */
#else
  #define COMPILER_MSVC /* Microsoft Visual C++ */
#endif

  #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef _QC
  #define COMPILER_MSQC /* Microsoft Quick C */
  #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif


#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef __BORLANDC__
  #define COMPILER_BORLAND /* Some Borland compiler */
  #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef __IBMC__
  #define COMPILER_IBM  /* IBM Visual Age for C++ */
  #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef __SC__
  #ifndef COMPILER_SYMANTEC
   #define COMPILER_SYMANTEC    /* Symantec C++ */
   #undef ___UNKNOWN_COMPILER_YET
  #endif
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef __ZTC__
  #define COMPILER_ZORTECH  /* Zortech C/C++ 3.x */
  #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef __WATCOMC__
  #define COMPILER_WATCOM   /* Watcom C/C++ */
  #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef __CC_ARM
  #define COMPILER_ARM  /* ARM C/C++ */
  #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef _TMS320C6X
   #if defined (_TMS320C6400)
   #define COMPILER_C64
   #undef ___UNKNOWN_COMPILER_YET
   #endif
   #if defined   (_TMS320C6400_PLUS)
   #define COMPILER_C64PLUS
   #undef ___UNKNOWN_COMPILER_YET
   #endif
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef __TMS320C55X__
  #define COMPILER_C55
  #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef __ADSPBLACKFIN__
  #define COMPILER_ADSP_BLACKFIN
  #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef __XCC__
   #define COMPILER_XTENSA
   #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
 #ifdef __GNUC__
   #ifdef __arm__
     #ifndef COMPILER_GNU_ARM
     #endif
     #define COMPILER_GNUARM /* GNU C/C++ compiler*/
   #else
     // GNU GCC x86 compiler
     #ifndef COMPILER_GNU
     #endif
     #define COMPILER_GNU /* GNU C/C++ */
   #endif
   #undef ___UNKNOWN_COMPILER_YET
 #endif
#endif

#ifdef ___UNKNOWN_COMPILER_YET
  #error  Unknown compiler
#endif


#endif //#ifndef COMPILER_ANSI

// ----------------------------------------------------------
//             Common types
// ----------------------------------------------------------
#if defined (COMPILER_GNU) | defined (COMPILER_GNUARM) | defined (COMPILER_XTENSA)
/*
  typedef signed char   int8_t;
  typedef unsigned char uint8_t;
*/
  #include <inttypes.h>
#elif defined (COMPILER_C64)
    #include <stdint.h>
#elif defined (COMPILER_C55)
    #include <stdint.h>
    typedef signed char   int8_t;
    typedef unsigned char uint8_t;
#elif defined(COMPILER_ADSP_BLACKFIN)
  typedef signed char   int8_t;
  typedef unsigned char uint8_t;
  typedef unsigned long  uint32_t;
  typedef unsigned short uint16_t;
  typedef long           int32_t;
  typedef short          int16_t;
  typedef long long          int64_t;
  typedef unsigned long long uint64_t;
  typedef uint32_t  uintptr_t;
#else
  typedef signed char   int8_t;
  typedef unsigned char uint8_t;
  typedef unsigned long  uint32_t;
  typedef unsigned short uint16_t;
  typedef long           int32_t;
  typedef short          int16_t;
  typedef __int64          int64_t;
  typedef unsigned __int64 uint64_t;
#endif



#if defined(COMPILER_CEARM9E)
typedef uint32_t uintptr_t;
#endif

#if defined(COMPILER_ARM)
typedef uint32_t uintptr_t;
#endif

typedef struct tagComplex16_t
{
  int16_t re;
  int16_t im;
} complex16_t;

typedef struct tagComplex32_t
{
  int32_t re;
  int32_t im;
} complex32_t;

// ----------------------------------------------------------
//        Language-dependent definitions
// ----------------------------------------------------------
#ifdef __cplusplus

  #undef  extern_C
  #define extern_C extern "C"

#else

  #undef extern_C
  #define extern_C

  #ifndef false
  #define false 0
  #endif
  #ifndef true
  #define true 1
  #endif

#endif

/*    Assertion support                   */
#if !defined(_ASSERT)
  #include <assert.h>
  #if defined(_DEBUG) && defined(COMPILER_MSVC)
    #define ASSERT(x)  assert(x)
  #else

//#undef ASSERT
#ifndef ASSERT
    #define ASSERT(_ignore)  ((void)0)
#endif

  #endif  /* _DEBUG */
#else  /* ASSERT*/
  #define ASSERT(exp) \
    {\
        extern void ExternalAssertHandler(void *, void *, unsigned);\
        (void)( (exp) || (ExternalAssertHandler(#exp, __FILE__, __LINE__), 0) );\
    }
#endif  /* ASSERT */


/*** Inline methods definition ***/
#undef inline_
#if (defined COMPILER_MSVC)||(defined COMPILER_CEARM9E)
  #define inline_ __inline
#elif defined (COMPILER_ADSP_BLACKFIN)
  #define inline_ inline
#elif defined(COMPILER_ANSI)
  #define inline_
#elif (defined COMPILER_GNU)||(defined COMPILER_GNUARM)||(defined COMPILER_ARM)
  #define inline_ static inline
#else
  #define inline_ static inline
#endif


#ifndef MAX_INT16
#define MAX_INT16 ((int16_t)0x7FFF)
#endif
#ifndef MIN_INT32
#define MIN_INT16 ((int16_t)0x8000)
#endif
#ifndef MIN_INT32
#define MAX_INT32 ((int32_t)0x7FFFFFFFL)
#endif
#ifndef MIN_INT32
#define MIN_INT32 ((int32_t)0x80000000L)
#endif

// size of variables in bytes
#ifdef COMPILER_C55
  #define SIZEOF_BYTE(x)  (sizeof(x)<<1)
#else
  #define SIZEOF_BYTE(x)  sizeof(x)
#endif

//---------------------------------------
// special keywords definition
// restrict  keyword means that the memory
//           is addressed exclusively via
//       this pointer
// onchip    keyword means that the memory
//           is on-chip and can not be
//           accessed via external bus
//---------------------------------------
#if   defined (COMPILER_C55)
  #define NASSERT _nassert
#elif defined (COMPILER_C64)
  #define onchip
  #define NASSERT _nassert
#elif defined (COMPILER_ADSP_BLACKFIN)
  #define onchip
  #define NASSERT(x) __builtin_assert(x)
#elif defined (COMPILER_GNUARM)
  #define onchip
  #define NASSERT(x) {(void)__builtin_expect((x)!=0,1);}
  #define restrict __restrict
#elif defined (COMPILER_GNU)
  #define onchip
  #define NASSERT(x)
  #define restrict __restrict
#elif defined (COMPILER_CEARM9E)
  #define onchip
  #define NASSERT(x)
  #define restrict
#else
  #define restrict
  #define onchip
  #define NASSERT ASSERT
#endif
#if defined (COMPILER_ADSP_BLACKFIN)
#define NASSERT_ALIGN(addr,align) __builtin_aligned(addr, align)
#else
#define NASSERT_ALIGN(addr,align) NASSERT(((uintptr_t)addr)%align==0)
#endif
#define NASSERT_ALIGN2(addr) NASSERT_ALIGN(addr,2)
#define NASSERT_ALIGN4(addr) NASSERT_ALIGN(addr,4)
#define NASSERT_ALIGN8(addr) NASSERT_ALIGN(addr,8)
#define NASSERT_ALIGN16(addr) NASSERT_ALIGN(addr,16)

//-------------------------------------------------------------------
// shoft float: 6 bits LSB - signed exponent, 10 bits MSB - mantissa
//-------------------------------------------------------------------
typedef int16_t tFloat ;
#define MAKE_FLOAT(mantissa,exponent) (((mantissa) & 0xFFC0)|(exponent&0x3F))

// ----------------------------------------------------------
// aligned data pointer: needed to align scratch memory blocks
// ----------------------------------------------------------
typedef union
{
  int8_t  int8;
  int16_t int16;
  int32_t int32;
}
tAlignedPtr;

/*---------------------------------------------------------
  Type definition fo double word memory access emulation
  (using LDDW/STDW instruction)
  ---------------------------------------------------------*/
#ifdef COMPILER_C64
  // Alias for C64's double type
  typedef double tDoubleWord;
//  typedef uint64_t tDoubleWord;
#elif defined (COMPILER_GNUARM) || defined (COMPILER_GNU)
typedef struct
{
  int32_t lo;
  int32_t hi;
} tDoubleWord __attribute__ ((aligned (8)));
#else

// for LITTLE ENDIAN format !!!!
typedef struct
{
  int32_t lo;
  int32_t hi;
} tDoubleWord;

#endif  // COMPILER_C64

// ----------------------------------------------------------
// Handle of scratch memory block
// ----------------------------------------------------------
typedef struct
{
 tAlignedPtr onchip * saram;
 size_t saramLength;        // sizeof of real memory allocated in SARAM
 tAlignedPtr onchip * daram;
 size_t daramLength;        // sizeof of real memory allocated in DARAM
}
tScratchMem;

//  default scratch memory blocks
extern tScratchMem* pDefaultScratchMem;

typedef int32_t f24;

#ifdef COMPILER_MSVC
    typedef __int64 f48;
    typedef __int64 i56;
#else
    typedef long long f48;
    typedef long long i56;
#endif

#if defined(__RENAMING__)
#include "__renaming__.h"
#endif

#ifdef COMPILER_XTENSA
    #include <xtensa/tie/xt_core.h>
    #include <xtensa/tie/xt_misc.h>
    #include <xtensa/tie/xt_hifi2.h>
    #include <stdint.h>
#endif

#endif //__NATUREDSPTYPES_H__
