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
#ifndef __HIFI2_COMMON_H__
#define __HIFI2_COMMON_H__

#include <assert.h>

#include "NatureDSP_Signal.h"
//#ifdef HIFI2_RENAMING__
//#include "hifi2_renaming__.h"
//#else
//#error !!!!!

//#endif

// ae_p16s
#define LD_P16F_I(   x, p, idx)   x = AE_LP16F_I    (   (ae_p16s *)(p), (idx)*(int)sizeof(ae_p16s))
#define LD_P16F_IU(  x, p, idx)       AE_LP16F_IU   (x, (ae_p16s *)(p), (idx)*(int)sizeof(ae_p16s))
#define ST_P16F_I(   x, p, idx)       AE_SP16F_L_I  (x, (ae_p16s *)(p), (idx)*(int)sizeof(ae_p16s))
#define ST_P16F_IU(  x, p, idx)       AE_SP16F_L_IU (x, (ae_p16s *)(p), (idx)*(int)sizeof(ae_p16s))

// ae_p24f
#define LD_P24F_I(   x, p, idx)   x = AE_LP24F_I    (   (ae_p24f *)(p), (idx)*(int)sizeof(ae_p24f))
#define LD_P24F_IU(  x, p, idx)       AE_LP24F_IU   (x, (ae_p24f *)(p), (idx)*(int)sizeof(ae_p24f))
#define ST_P24F_I(   x, p, idx)       AE_SP24F_L_I  (x, (ae_p24f *)(p), (idx)*(int)sizeof(ae_p24f))
#define ST_P24F_IU(  x, p, idx)       AE_SP24F_L_IU (x, (ae_p24f *)(p), (idx)*(int)sizeof(ae_p24f))


// ae_p24f
#define LD_P24S_I(   x, p, idx)   x = AE_LP24_I     (   (ae_p24s *)(p), (idx)*(int)sizeof(ae_p24s))
#define LD_P24S_IU(  x, p, idx)       AE_LP24_IU    (x, (ae_p24s *)(p), (idx)*(int)sizeof(ae_p24s))
#define ST_P24S_I(   x, p, idx)       AE_SP24S_L_I  (x, (ae_p24s *)(p), (idx)*(int)sizeof(ae_p24s))
#define ST_P24S_IU(  x, p, idx)       AE_SP24S_L_IU (x, (ae_p24s *)(p), (idx)*(int)sizeof(ae_p24s))

// ae_p24x2f
#define LD_P24X2F_I( x, p, idx) x = AE_LP24X2F_I  (   (ae_p24x2f *)(p), (idx)*(int)sizeof(ae_p24x2f))
#define LD_P24X2F_IU(x, p, idx)     AE_LP24X2F_IU (x, (ae_p24x2f *)(p), (idx)*(int)sizeof(ae_p24x2f))
#define LD_P24F_I(   x, p, idx) x = AE_LP24F_I    (   (ae_p24f   *)(p), (idx)*(int)sizeof(ae_p24f))
#define LD_P24F_IU(  x, p, idx)     AE_LP24F_IU   (x, (ae_p24f   *)(p), (idx)*(int)sizeof(ae_p24f))


// ae_q32f
#define LD_Q32_I(    x, p, idx) x = AE_LQ32F_I    (   (ae_q32s   *)(p), (idx)*(int)sizeof(ae_q32s))
#define LD_Q32F_IU(  x, p, idx)     AE_LQ32F_IU   (x, (ae_q32s   *)(p), (idx)*(int)sizeof(ae_q32s))

#define ST_P24X2F_I( x, p, idx)     AE_SP24X2F_I  (x, (ae_p24x2f *)(p), (idx)*(int)sizeof(ae_p24x2f))
#define ST_P24X2F_IU(x, p, idx)     AE_SP24X2F_IU (x, (ae_p24x2f *)(p), (idx)*(int)sizeof(ae_p24x2f))

#define ST_Q32S_I(   x, p, idx)       AE_SQ32F_I(  x, (ae_q32s   *)(p), (idx)*sizeof(ae_q32s))

#define LD_Q56S_I(   x, p, idx)   x = AE_LQ56_I   (   (ae_q56s *)(p), (idx)*(int)sizeof(ae_q56s))
#define LD_Q56S_IU(  x, p, idx)       AE_LQ56_IU  (x, (ae_q56s *)(p), (idx)*(int)sizeof(ae_q56s))
#define ST_Q56S_I(   x, p, idx)       AE_SQ56S_I  (x, (ae_q56s *)(p), (idx)*(int)sizeof(ae_q56s))
#define ST_Q56S_IU(  x, p, idx)       AE_SQ56S_IU (x, (ae_q56s *)(p), (idx)*(int)sizeof(ae_q56s))

// testing stuff
#define load_24f(p)     AE_LP24F_I  (   (const ae_p24f *)(p), 0)  // load  x      -> {x, x}
#define store_24f(x, p) AE_SP24F_L_I(x, (      ae_p24f *)(p), 0)  // store {h, l} -> l

#define load_56s(p)     AE_LQ56_I   (   (const ae_q56s *)(p), 0)
#define store_56s(x, p) AE_SQ56S_I  (x, (      ae_q56s *)(p), 0)

#define mul_24i(     x, y)  AE_MULP24S_HH(x, y)
#define mul_24f(     x, y)  AE_MULFP24S_HH(x, y)
#define mac_24i(acc, x, y)  AE_MULAP24S_HH(acc, x, y)
#define mac_24f(acc, x, y)  AE_MULAFP24S_HH(acc, x, y)
#define msb_24i(acc, x, y)  AE_MULSP24S_HH(acc, x, y)
#define msb_24f(acc, x, y)  AE_MULSFP24S_HH(acc, x, y)

#define IS_ALIGN(p) ((((int)(p))&0x7) == 0)
#define IS_ALIGN_4(p) ((((int)(p))&0x3) == 0)

#ifdef _MSC_VER
    #define ALIGN(x)    _declspec(align(x))
#else
    #define ALIGN(x)    __attribute__((aligned(x)))
#endif

#define INV_TBL_BITS 7
extern const int32_t tab_invQ30[128];

#if XCHAL_HAVE_NSA
  #define NSA(n) XT_NSA(n)
#else
  inline_ int32_t NSA(int32_t n)
  {
    ae_q56s t;
    if (!n) return 31;
    t = AE_CVTQ48A32S(n);
    return AE_NSAQ56S(t)-8;
  }
#endif

/*****************************************************************************
  1 º¯ÊýÉùÃ÷
*****************************************************************************/
/*void fir_bk (
            int16_t *pshwOut,   // [N]
            int16_t *pshwIn,    // [M+N-1]
            int16_t *pshwCoef,  // [M]
            int N,
            int M);

void vec_divide (
          f24   *restrict frac, // [N]
          short *restrict exp,  // [N]
    const f24   *restrict x,    // [N]
    const f24   *restrict y,    // [N]
    int N
);

// returns packed result, exp resides in 8 most significant bits
int32_t scl_divide(f24 x, f24 y);*/

#endif
