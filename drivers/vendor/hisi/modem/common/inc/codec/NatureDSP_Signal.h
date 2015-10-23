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
#ifndef __NATUREDSP_SIGNAL_H__
#define __NATUREDSP_SIGNAL_H__

#include "NatureDSP_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
  FIR filters:
  fir_bk     Block real FIR
  fir_cbk    Complex block FIR
  fir_sr     Symetrical real FIR
  fir_dec    Decimating FIR filer
  fir_interp Interpolating FIR filter
  fir_ss     Single-sample real FIR
  fir_convol Convolution
  fir_xcorr  Correlation
  fir_acorr  Autocorrelation
  fir_dlms   Raw single sample delayed LMS algorithm
  fir_blms   Blockwise Adaptive LMS algorithm

  General note on filters:

  Performance of filtering functions for short order (20…32) filters  or short
  processing blocks (<64) may be degraded due to the overhead for delay line
  moves (xxx_put() functions). To eliminate this overhead, user may specify
  alternative storage method (smethod==1) for delay lines. This method
  approximately doubles the memory requirements but reduces CPU cycles needed
  for xxx_put() functions.

===========================================================================*/

/*-------------------------------------------------------------------------
  Real FIR filter.
  Computes a real FIR filter (direct-form) using IR stored in vector h.
  The real data input is stored in vector x. The filter output result is
  stored in vector r. The filter calculates N output samples using M
  coefficients and requires last M+N-1 samples on the input.

  Input:
  x[N+M-1]  - input data. First in time corresponds to x[0], Q31
  h[M]      - filter coefficients in normal order, Q31
  N         - length of sample block
  M         - length of filter
  Output:
  y[N]      - output data, Q31

  Restriction:
  x,h,r should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,h - should be aligned by 8 bytes
  N,M - divisible by 4 and >8
-------------------------------------------------------------------------*/
/*void fir_bk (
            f24 *restrict y, // [N]
    const   f24 *restrict x, // [M+N-1]
    const   f24 *restrict h, // [M]
            int N,
            int M
);
*/
void fir_bk (
            int16_t *pshwOut,   // [N]
            int16_t *pshwIn,    // [M+N-1]
            int16_t *pshwCoef,  // [M]
            int N,
            int M);
/*-------------------------------------------------------------------------
Real FIR filter: object interface

Object allocation: bkfir_alloc()
Input:
N		length of sample block
M		length of filter
Returns: size of memory in bytes to be allocated

Object initialization: bkfir_init()
Input:
objmem	allocated memory block
h[M]	filter coefficients; h[0] is to be multiplied with the newest sample
N		length of sample block
M		length of filter
Returns: handle to the object

Put samples to delay line	bkfir_put()
Input:
x[N]	input samples
Returns: none

Process samples	bkfir_process()
Output:
y[N]	output samples
Returns: none
-------------------------------------------------------------------------*/
typedef void* bkfir_handle_t;
size_t bkfir_alloc(int N, int M, int smethod);
bkfir_handle_t bkfir_init(void * objmem, int N, int M, int smethod,const f24 * restrict h);
void bkfir_put( bkfir_handle_t handle, const f24 * x );
void bkfir_process( bkfir_handle_t handle, f24 * restrict y );

/*-------------------------------------------------------------------------
Block real FIR filter with circular delay line
Computes a real FIR filter (direct-form) using IR stored in vector h. From
functional point of view it provides similar algorithm as bkfir_xxx(), however
it uses circular delay line to store input samples. The filter calculates N
output samples using M coefficients and requires delay line of length M. This
kind of filter minimizes the overhead introduced by moving the delay line and
looping
Other differences are:
-	user is responsible for alignment and initialization
-	put/process functions are combined
-	last 3 coefficients of IR should be zeros
-	user allocates memory for delay line and stores filter IR in a memory in
    proper way
-	IR is stored in duplicated fashion, so second copy of IR should be placed
    immediately after original coefficients
-	size of data block and filter length should be a multiple of 4

Input/output:
z[M]    delay line. Should be zeroed before the first call
Input:
x[N]    input data block
h[M*2]  duplicated filter coefficients
p       position of beginning the convolution.
N       length of sample block
M       length of filter
Output:
y[N]    output data

Returns: updated value of p which is (p+M) modulo M

Restrictions:
x,y,z,h   should not overlap
x,y,z,h   aligned on a 8-bytes boundary
p,N,M     multiples of 4

-------------------------------------------------------------------------*/
int bkfircirc
      (      f24 * restrict z,
             f24 * restrict y,
             const f24 * restrict x,
             const f24 * restrict h,
             int p,
             int N,
             int M);

/*-------------------------------------------------------------------------
  Complex FIR filter.
  Computes a complex FIR filter (direct-form) using complex IR stored in
  vector h. The complex data input is stored in vector x. The filter output
  result is stored in vector r. The filter calculates N output samples using
  M coefficients and requires last M+N-1 samples on the input. Real and
  imaginary parts are interleaved and real parts go first (at even indexes).

  Input:
  x[2*(N+M-1)]  - input data. First in time corresponds to x[0], Q31
  h[2*M]        - complex filter coefficients in normal order, Q31
  N             - length of sample block
  M             - length of filter
  smethod		- storage method
                  0 – normal storage
                  1 – alternative method
  Output:
  y[2*N]        - output complex data, Q31

  Restrictions:
  x,h,r should not be overlapped

  PERFORMANCE NOTE:
  x,h - should be aligned by 8 bytes
  N,M - divisible by 4 and >8
-------------------------------------------------------------------------*/
void fir_cbk (
          f24   *restrict y, // [2*N]
    const f24   *restrict x, // [2*(N+M-1)]
    const f24   *restrict h, // [2*M]
    int N,
    int M
);

/*-------------------------------------------------------------------------
  Complex FIR filter. object interface

Object allocation	cxfir_alloc()
Input:
N		length of sample block (in complex samples)
M		length of filter
Returns: size of memory in bytes to be allocated

Object initialization	cxfir_init()
Input:
objmem		allocated memory block
h[2*M]	complex filter coefficients; h[0] is to be multiplied with the newest sample
N		length of sample block
M		length of filter
smethod storage method
        0 – normal storage
        1 – alternative method
Returns: handle to the object

Put samples to delay line	cxfir_put()
Input:
x[2*N]	input samples
Returns: none

Process samples	cxfir_process()
Output:
y[2*N]	output samples
Returns: none
-------------------------------------------------------------------------*/
typedef void* cxfir_handle_t;
size_t cxfir_alloc(int N, int M, int smethod);
cxfir_handle_t cxfir_init(void * objmem, int N, int M, int smethod, const f24 * restrict h);
void cxfir_put(cxfir_handle_t handle, const f24 * x );
void cxfir_process(cxfir_handle_t handle, f24 * restrict r );

/*-------------------------------------------------------------------------
  Symmetrical block real FIR filter
  Input:
  x[N+M-1]  - input data. First in time corresponds to x[0]
  h[(M+1)/2]- a half of filter coefficients in normal order
  N         - length of sample block
  M         - length of filter
  smethod   - storage method
              0 – normal storage
              1 – alternative method
  Output:
  y[N]      - output data

  Restriction:
  x,h,r should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,h - should be aligned by 8 bytes
  N,M - divisible by 4 and >8
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void fir_sr (
            f24   *restrict y, // [N]
    const   f24   *restrict x, // [N+M-1]
    const   f24   *restrict h, // [(M+1)/2]
    int N, int M
);

/*-------------------------------------------------------------------------
  Symmetrical block real FIR filter: object interface

Object allocation	srfir_alloc()
Input:
N		length of sample block
M		length of filter
Returns: size of memory in bytes to be allocated

Object initialization	srfir_init()
Input:
objmem		allocated memory block
h[(M+1)/2]	the half of filter coefficients. Last tap is suggested to be a center of IR
N		length of sample block
M		length of filter
Returns: handle to the object

Put samples to delay line	srfir_put()
Input:
x[N]	input samples
Returns: none

Process samples	srfir_process( )
Output:
y[N]	output samples
Returns: none
-------------------------------------------------------------------------*/
typedef void* srfir_handle_t;
size_t srfir_alloc(int N, int M, int smethod);
srfir_handle_t srfir_init(void * objmem,int N, int M, int smethod, const f24 * restrict h);
void srfir_put( srfir_handle_t handle, const f24 * x );
void srfir_process( srfir_handle_t handle, f24 * restrict y );

/*-------------------------------------------------------------------------
  Decimating block real FIR filter
  Input:
  x[D*N+M-1]- input data. First in time corresponds to x[0]
  h[M]      - filter coefficients in normal order
  N         - length of output sample block
  M         - length of filter
  D         - decimation factor
  Output:
  y[N]      - output data

  Restrictions:
  1. x,h,r should not be overlapped
  2. D should be >1

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,h - should be aligned by 8 bytes
  N,M - divisible by 4 and >8
  D should be 2,3 or 4
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void fir_dec (
          f24   *restrict y, // [N]
    const f24   *restrict x, // [D*N+M-1]
    const f24   *restrict h, // [M]
    int N, int M, int D
);
/*-------------------------------------------------------------------------
  Decimating block real FIR filter: object interface

Object allocation	firdec_alloc()
Input:
D		decimation factor (has to be 2, 3 or 4)
N		length of output sample block
M		length of filter
Returns: size of memory in bytes to be allocated

Object initialization	firdec_init()
Input:
objmem		allocated memory block
h[M]	filter coefficients; h[0] is to be multiplied with the newest sample
D		decimation factor (has to be 2, 3 or 4)
N		length of output sample block
M		length of filter
Returns: handle to the object

Put samples to delay line	firdec_put()
Input:
x[D*N]	input samples
Returns: none

Process samples	firdec_process()
Output:
y[N]	output samples
Returns: none
-------------------------------------------------------------------------*/
typedef void* firdec_handle_t;
size_t firdec_alloc(int D, int N, int M);
firdec_handle_t firdec_init(void * objmem, int D, int N, int M, const f24 * restrict h);
void firdec_put(firdec_handle_t handle, const f24 * x );
void firdec_process(firdec_handle_t handle, f24 * restrict y );

/*-------------------------------------------------------------------------
  Interpolating block real FIR filter
  Input:
  x[N+M*D-1]- input data. First in time corresponds to x[0]
  h[M*D]    - subfilter coefficients. Filter coefficients should be decimated
              by D and resulted D subfilters should be placed one by another
  N         - length of sample block
  M         - length of subfilter
  D         - upsample factor
  Output:
  y[N*D]    - output data


  Restrictions:
  1. x,h,y should not be overlapped
  2. D should be >1

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,h - should be aligned by 8 bytes
  M,N - divisible by 4 and >8
  D should be 2,3 or 4
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void fir_interp (
          f24   *restrict y,  // [N*D]
    const f24   *restrict x,  // [N+M*D-1]
    const f24   *restrict h,  // [D*M]
    int N, int M, int D
);

/*-------------------------------------------------------------------------
  Interpolating block real FIR filter: object interface

Object allocation	firinterp_alloc()
Input:
D		interpolation ratio(has to be 2, 3, 4)
N		length of input sample block
M		llength of subfilter. Total length of filter is M*D
Returns: size of memory in bytes to be allocated

Object initialization	firinterp_init()
Input:
objmem		allocated memory block
h[M]	filter coefficients; h[0] is to be multiplied with the newest sample
D		interpolation ratio(has to be 2, 3, 4)
N		length of input sample block
M		length of subfilter. Total length of filter is M*D
Returns: handle to the object

Put samples to delay line	firinterp_put()
Input:
x[N]	input samples
Returns: none

Process samples	firinterp_process()
Output:
y[N*D]	output samples
Returns: none
-------------------------------------------------------------------------*/
typedef void* firinterp_handle_t ;
size_t firinterp_alloc(int D, int N, int M);
firinterp_handle_t firinterp_init(void * objmem, int D, int N, int M, const f24 * restrict h);
void firinterp_put(firinterp_handle_t handle, const f24 * x );
void firinterp_process(firinterp_handle_t handle, f24 * restrict y );

/*-------------------------------------------------------------------------
  Single-sample real FIR filter.
  Passes one sample via real FIR filter (direct-form) using IR stored in
  vector h and moves delay line with samples. The real data input (delay line)
  is stored in vector x. The filter output result is stored in vector r.
  The filter calculates one output sample using M coefficients and requires
  M samples on the input.
  NOTE:
  The difference with fir_bk() function is that this function returns sample
  in Q30, not Q31 format.

  Input:
  x[M]      - input data. First in time corresponds to x[0],Q31
  h[M]      - filter coefficients in normal order,Q31
  M         - length of filter

  Output:
  x         - shifted in time input samples by 1 sample.

  Returned value:
  filtered data,Q30

  Restriction:
  x,h should not be overlapped
  M>0

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,h - should be aligned by 8 bytes
  M - divisible by 4 and >8
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
f24 fir_ss (
          f24 *restrict x, // [M]
    const f24 *restrict h, // [M]
          int M
);

/*-------------------------------------------------------------------------
  Performs convolution between vectors x and y. Algebraically, convolution
  is the same operation as multiplying the polynomials whose coefficients
  are the elements of x and y.
  The convolution calculates M+N-1 output samples using vectors of size N
  and M correspondingly.
  Function is equivalent to MATLAB’s conv() function operating on real
  data.

  Input:
  x[N]      - input data. First in time corresponds to x[0]
  y[M]      - impulse response
  N         - length of x
  M         - length of y
  Output:
  r[M+N-1]  - output data

  Restriction:
  x,y,r should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,y - should be aligned by 8 bytes
  N,M - divisible by 4 and >8
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void fir_convol ( f24 * restrict r,
            const f24 * restrict x,
            const f24 * restrict y,
            int N,
            int M);



/*-------------------------------------------------------------------------
  Correlation (similar to MATLABs xcorr()).
  Estimates the cross-correlation between vectors x and y.
  The cross-correlation calculates M+N-1 output samples using vectors of
  size N and M correspondingly. Function is equivalent to MATLAB's xcorr()
  function operating on real data. It is also similar fir_convol() to but
  operates on reversed y sequence.

  Input:
  x[N],y[M] - input data, Q31
  N,M       - length of input vectors
  Output:
  r[M+N-1]  - output data, Q31

  Restriction:
  x,y,r should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,h - should be aligned by 8 bytes
  N,M - divisible by 4 and >8
-------------------------------------------------------------------------*/
void fir_xcorr (  f24 * restrict r,
            const f24 * restrict x,
            const f24 * restrict y,
            int N,
            int M);


/*-------------------------------------------------------------------------
  Autocorrelation (similar to MATLABs xcorr(x)).
  Estimates the auto-correlation between of vector x. Returns autocorrelation
  in a length N vector, where x is length N vector. This function is similar
  to MATLAB's xcorr() function operating on real data with one argument.
  However, MATLAB xcorr() function always returns autocorrelation of length
  2*N-1 but it is centered and symmetrical. In contrary, fir_acorr() function
  returns only the half of autocorrelation result.

  Input:
  x[N] - input data, Q31
  M    - length of input vector
  Output:
  y[N]  - output data, Q31

  Restriction:
  x,y should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x - should be aligned by 8 bytes
  N - divisible by 4 and >8
-------------------------------------------------------------------------*/
void fir_acorr (
          f24   *restrict y, // [N]
    const f24   *restrict x, // [N]
    int N
);

/*-------------------------------------------------------------------------
  Raw single sample delayed LMS algorithm

  Input:
  x[N+1]    - data vector. First in time value is in x[1] and previosly
              saved sample (from last step) in x[0]
  h[N]      - impulse response, Q31
  b         - scaled error from previous step (Q32)
  N         - length of vectors
  returned value:
  error in Q30

  Restrictions:
  all arrays should not be overlapping

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,h   - should be aligned by 8 bytes
  N     - divisible by 4 and >8
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
f24 fir_dlms(
          f24 *restrict h, // [N]
    const f24 *restrict x, // [N+1]
    f24 b,  int N
);

/*-------------------------------------------------------------------------
  Blockwise Adaptive LMS Least Mean Square. Function calculates error
  vector e=r-x*h and adapts IR in such a way to minimize error output
  Alorithm includes FIR filtering and calculation of correlation between
  error output and input signal

  Input:
  r[N]      - input (near end) data vector. First in time value is in x[0]
  x[N+M]    - input (far end) data vector. First in time value is in x[0]
  h[M]      - impulse response
  norm      - normalization factor: power of signal multiplied by N, Q31
  mu        - adaptation coefficient, Q31.
  N         - length of vectors
  M         - length of impulse response
  Output:
  e[N]      - error output
  h[M]      - updated impulse response

  Restrictions:
  all arrays should not be overlapping

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,r,h,e   - should be aligned by 8 bytes
  N,M       - divisible by 4 and >8
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void fir_blms (
          f24 *restrict e, // [N]    out
          f24 *restrict h, // [M] in/out
    const f24 *restrict r, // [N]
    const f24 *restrict x, // [N+M]
          f24 norm, // Q31
          f24 mu,   // Q31
          int N, int M
);


/*-------------------------------------------------------------------------
Blockwise LMS algorithm performs filtering of input samples, computation of
error over a block of reference samples and makes blockwise update of IR to
minimize the error output.
Algorithm includes FIR filtering, calculation of correlation between the
error output and reference signal and IR taps update based on that
correlation.
NOTES:
1.	For N=1 this algorithm is equivalent to standard LMS however bigger block
    size reduce the computational overhead keeping adaptation properties.
2.	Right selection of N depends on the change rate of impulse response.
    However on static or slow varying channels convergence rate depends on
    selected mu and M, but not on N
3.	Algorithm uses separated calculation of adaptation rate because for many
    applications like modem equalizers the power of reference signal is known
    in advance and this step may be omitted saving the computation load.
    Also, in some applications (like echo-cancellers) user may want to
    disable adaptation temporarily however calculating error output. For that
    simply set mu to zero.
4.	Proper scaling on reference signal input affects to accuracy of
    adaptation on low signal levels. If reference input is non-stationary
    signal, bad scaling may cause extra brown noise on error output. To avoid
    this, set ascale variable to 1 during initialization. If it will be set to
    zero, LMS will compute the scale during the first call of lms_put() and
    later will not adapt this scale factor saving computational resources.
5.	Normalization factor (power of reference signal multiplied by N) normally
    may be calculated simply by vec_bexp() function. However, for non-
    stationary reference signals other algorithms may be applied to avoid
    divergence on weak reference signals (i.e. peak leveling, adding small
    constant, Geigel-like adaptation rate control, etc.)
6.	Functions use HiFi2 56-bit accumulators for all computational steps. So,
    user has to avoid very long blocks to prevent overflows during IR update
    step. Typically, N should not exceed 128 to guarantee 56-bit range.

From the user perspective, function assumes following representation for data:
impulse response                                Q31
input samples, reference signal, error signal	Qx
LMS step                                        Q15
normalization factor                            Q(2x -32)


Object allocation	lms_alloc()
Input:
N		length of input sample block
M		length of IR
Returns: size of memory in bytes to be allocated

Object initialization	lms_init()
Input:
objmem		allocated memory block
N		length of input sample block
M		length of IR
ascale  if nonzero, use autoscaling on reference input. Otherwise scale is computed on the first call if lms_put()
h[2*M]	starting values for IR. If NULL, all taps are zeroed
Returns: handle to the object

Get impulse response	lms_getIR()
Returns: pointer to IR

Put samples to delay line	lms_put()
Input:
x[2*N]	input reference samples, Qx
Returns: none

Update adaptation rate	lms_updateMu()
Input:
norm	normalization factor: power of reference signal multiplied by N, Q(2x-48)
mu		adaptation coefficient (LMS step), Q15
Returns: none

Compute LMS  error and adapt impulse response	lms_process ()
Input:
r[2*N]	input signal, Qx
Output:
e[2*N]	calculated error, Qx
Returns: none

  Restrictions:
  M*N should be less that 32767
  h,x,r,y,e - should not overlap

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  N,M       - divisible by 4 and >8
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
typedef void* lms_handle_t ;
size_t lms_alloc(int N, int M);
lms_handle_t lms_init(void * objmem, int N, int M,int ascale,const f24* h);
const f24* lms_getIR(lms_handle_t handle);
void lms_put(lms_handle_t handle, const f24 * x );
void lms_updateMu(lms_handle_t handle, int32_t norm,int16_t mu);
void lms_process (lms_handle_t handle,f24 * restrict e,const f24 * restrict r);

/*===========================================================================
  IIR filters:
  iir_bqc   Biquad Complex block IIR
  iir_bqd   Biquad Double precision IIR filter
  iir_latr  Lattice block Real IIR
  iir_latc  Lattice complex block IIR
  iir_latd  Lattice double precision IIR filter
===========================================================================*/

/*-------------------------------------------------------------------------
  Lattice block Real IIR. Makes filtering using lattice all-pole AR model.
  Function iir_latr makes single precision calculation
  Input:
  x[N]    - input data
  refl[M] - Reflection coefficients (Q31 format)
  N       - length of input vector
  M       - filter order
  Output:
  r[N]   - output data
  b[M+1] - Delay line elements from previous call. Should be zeroed prior
           to the first call.

  Restriction:
  x,refl,r should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  r,b,x,refl  - should be aligned by 8 bytes
  M  - should be in range 1...6
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void iir_latr (
          f24 *restrict r,      // [N]   out
          f24 *restrict b,      // [M+1] in/out
    const f24 *restrict x,      // [N]
    const f24 *restrict refl,   // [M]
    int N, int M
);

/*-------------------------------------------------------------------------
  Lattice block Real IIR. Makes filtering using lattice all-pole AR model.
  Uses wider multiplications on each stage that improves stability and dynamic
  range on the low signals
  In contrast to iir_latr() it performs some computations in double precision
  and stores delay line elements in Q63 format.

  Input:
  x[N]    - input data
  refl[M] - Reflection coefficients (Q31 format)
  N       - length of input vector
  M       - filter order
  Output:
  r[N]   - output data
  b[M+1] - Delay line elements from previous call. Should be zeroed prior
           to the first call.

  Restriction:
  x,refl,r should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  r,b,x,refl  - should be aligned by 8 bytes
  N  - divisible by 4 and >8
  M  - should be in range 1...6
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void iir_latd (
          f24 *restrict r,      // [N]   out
          i56 *restrict b,      // [M+1] in/out
    const f24 *restrict x,      // [N]
    const f24 *restrict refl,   // [M]
    int N, int M
);

/*-------------------------------------------------------------------------
  Lattice block Real IIR. object interface

Object allocation	latr_alloc()
Input:
N		length of input sample block
M		number of sections
prec	precision of algorithm and intermediate data (0 – single precision,
        1 – double precision)
Returns: size of memory in bytes to be allocated

Object initialization	latr_init()
Input:
objmem		allocated memory block
N		length of input sample block
M		length of IR
prec	precision of algorithm and intermediate data (0 – single precision,
        1 – double precision)
k[M]	reflection coefficients, Q31
Returns: handle to the object

Process samples	latr_process ()
Input:
x[N]	input samples, Q31
Output:
r[N]	output data, Q31
Returns: none

Returned value	none
-------------------------------------------------------------------------*/
typedef void* latr_handle_t;
size_t latr_alloc(int N, int M, int prec);
latr_handle_t latr_init(void * objmem, int N, int M, int prec,const f24 * restrict k);
void latr_process (latr_handle_t handle, f24 * r, const f24 * x );

/*-------------------------------------------------------------------------
  Lattice block complex IIR. Makes filtering using lattice all-pole AR model

  Input:
  x[2*N]     - input data
  refl[M]    - Reflection coefficients (Q31 format)
  N          - length of input vector (number of complex data points)
  M          - filter order
  Output:
  r[2*N]     - output data
  b[2*(M+1)] - Delay line elements from previous call. Should be zeroed prior
               to the first call.

  Restriction:
  x,refl,r should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,r,b,refl  - should be aligned by 8 bytes
  M   - from the range 1...6

  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void iir_latc (
          f24 *restrict r,      // [2*N] out
          f24 *restrict b,      // [M+1] in/out
    const f24 *restrict x,      // [2*N]
    const f24 *restrict refl,   // [M]
    int N, int M
);

/*-------------------------------------------------------------------------
  Lattice block complex IIR. object interface

Object allocation	latc_alloc
Input:
N		length of input sample block
M		number of sections
Returns: size of memory in bytes to be allocated

Object initialization	latc_init()
Input:
objmem		allocated memory block
N		length of input sample block
M		length of IR
k[M]	reflection coefficients, Q31
Returns: handle to the object

Process samples	latc_process ()
Input:
x[2*N]	input samples, Q31
Output:
r[2*N]	output data, Q31
Returns: none
Returned value	none
-------------------------------------------------------------------------*/
typedef void* latc_handle_t;
size_t latc_alloc(int N, int M);
latc_handle_t latc_init(void * objmem, int N, int M, const f24 * restrict k);
void latc_process (latc_handle_t handle, f24 * r, const f24 * x );

/*-------------------------------------------------------------------------
  Biquad complex block IIR filter (cascaded IIR direct form II using 5
  coefficients per biquad)

  Input:
  x[2*N]     - input data
  coef[6*M]  - Coefficients. Coefficients order is: g b0 b1 b2 a1 a2 ...
               a1,a2,b1,b2,b0 - Q30
               g              - Q31
  gain         total gain, Q8.23
  N          - length of input vector (number of complex data points)
  M          - filter order (number of biquad stages)
  Output:
  r[2*N]     - output data
  b[4*(M+1)] - Delay line elements from previous call. Should be zeroed prior
               to the first call.

  Restriction:
  x,coef,r,d should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  r,d,coef,x  - should be aligned by 8 bytes
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void iir_bqc (
          f24 *restrict r,        // [2*N]
          f24 *restrict d,        // [4*(M+1)]  in/out
    const f24 *restrict x,        // [2*N]
    const f24 *restrict coef,     // [6*M]
          int32_t gain,                   // Q8.24
          int N, int M
);
/*-------------------------------------------------------------------------
  Biquad complex block IIR filter: object interface

Object allocation	size_t bqñiir_alloc()
Input:
N		length of input sample block
M		number of bi-quad sections
Returns: size of memory in bytes to be allocated

Object initialization	bqñiir_init()
Input:
objmem		allocated memory block
N		length of input sample block
M		length of IR
coef[M*6]	filter coefficients stored in blocks of 6 numbers: g b0 b1 b2 a1 a2.
            Fractional formats used:
            b0 b1 b2 a1 a2	Q30
            g             	Q31
gain		total gain, Q8.23
Returns: handle to the object

Process samples	bqñiir_process()
Input:
x[2*N]	input samples, Q31
Output:
r[2*N]	output data, Q31
Returns: none

Returned value	none
-------------------------------------------------------------------------*/
typedef void* bqciir_handle_t;
size_t bqciir_alloc (int N, int M);
bqciir_handle_t bqciir_init(void * objmem, int N, int M, const f24 * restrict coef,int32_t gain);
void bqciir_process(bqciir_handle_t handle, f24 * restrict r,const f24 * x);

/*-------------------------------------------------------------------------
  Biquad real double precision block IIR filter (cascaded IIR direct form
  II using 5 coefficients per biquad)

  Input:
  x[N]       - input data
  coef[6*M]  - Coefficients. Coefficients order is: g b0 b1 b2 a1 a2 ...
               a1,a2,b1,b2,b0 - Q30
               g              - Q31
  gain         total gain, Q8.23
  N          - length of input vector (number of complex data points)
  M          - filter order (number of biquad stages)
  Output:
  r[N]       - output data
  b[2*(M+1)] - Delay line elements from previous call. Should be zeroed prior
               to the first call.

  Restriction:
  x,coef,r,d should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  d,coef  - should be aligned by 8 bytes
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void iir_bqd (
          f24 * restrict r,     // [N]
          f24 * restrict d,     // [2*(M+1)]  in/out
    const f24 * restrict x,     // [N]
    const f24 * restrict coef,  // [6*M]
          int32_t gain,           // Q8.24
          int N, int M
);

/*-------------------------------------------------------------------------
  Biquad complex block IIR filter: object interface

Object allocation	size_t bqriir_alloc()
Input:
N		length of input sample block
M		number of bi-quad sections
Returns: size of memory in bytes to be allocated

Object initialization	bqriir_init()
Input:
objmem		allocated memory block
N		length of input sample block
M		length of IR
coef[M*6]	filter coefficients stored in blocks of 6 numbers: g b0 b1 b2 a1 a2.
            Fractional formats used:
            b0 b1 b2 a1 a2	Q30
            g             	Q31
gain		total gain, Q8.23
Returns: handle to the object

Process samples	bqriir_process()
Input:
x[N]	input samples, Q31
Output:
r[N]	output data, Q31
Returns: none

Returned value	none
-------------------------------------------------------------------------*/
typedef void* bqriir_handle_t ;
size_t bqriir_alloc(int N, int M);
bqriir_handle_t bqriir_init(void * objmem, int N, int M, const f24 * restrict coef,int32_t gain);
void bqriir_process( bqriir_handle_t handle, f24 * restrict r,const f24 * x);

/*===========================================================================
    Vector mathematics:
    vec_dot       Vector dot product
    vec_add       Vector add
    vec_power     Sum of squares over a vector
    vec_bexp      Exponent of all values in a vector
    vec_logn      Natural log of a vector
    vec_log2      Log base 2 of a vector
    vec_log10     Log base 10 of a vector
    vec_recip16   Vector reciprocal
    vec_divide    Vector divide
    mtx_mpy       Matrix multiply
    mtx_trans     Matrix transpose
===========================================================================*/


/*-------------------------------------------------------------------------
  Vector dot product
  Input:
  x[N]      - input data,Q31
  y[N]      - input data,Q31
  N         - length of vectors
  returned value:
  dot product of all data pairs (Q30)

  Restriction:
  none

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,y - should be aligned by 8 bytes
  x,y - should not be overlapping
  N   - divisible by 2
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
f24 vec_dot (
    const f24 *restrict x,  // [N]
    const f24 *restrict y,  // [N]
    int N
);

/*-------------------------------------------------------------------------
  Vector summation.
  Input:
  x[N]      - input data.
  y[N]      - input data.
  N         - length of vectors
  Output:
  z[N]      - sum of x and y

  Restriction:
  x,y,z - should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  z,x,y - should be aligned by 8 bytes
  N   - divisible by 2
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void vec_add (
          f24 *restrict z,
    const f24 *restrict x,
    const f24 *restrict y,
    int N
);

/*-------------------------------------------------------------------------
  This routine computes power of vector with scaling output result by rsh
  bits. Accumulation is done in 56-bit wide accumulator and output may
  scaled down with saturation by rsh bits. So, if representation of x
  input is Qx, result will be represented in Q(2x-rsh) format.
  Input:
  x[N]      - input data, Qx
  N         - length of vectors
  returned value:
  Sum of squares of a vector, Q(2x-rsh)

  Restriction:
  none

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x   - should be aligned by 8 bytes
  N   - divisible by 2
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
int32_t vec_power (
    const f24 *restrict x, // [N]
    int rsh,
    int N
);

/*-------------------------------------------------------------------------
  Vector scaling with saturation

  This routine makes shift with saturation of data values in the vector
  by given scale factor (degree of 2)
  Function vec_scale() makes multiplication of Q31 vector to Q31 coefficient
  which is not a power of 2 forming Q31 result.

  Input:
  x[N]      - input data.
  t         - shift count. If positive, it shifts left with saturation,
              if negative it shifts right
  s         - coefficient, Q31
  N         - length of vectors
  returned value:
  none

  Restriction:
  arrays should not be overlapped

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,y - should be aligned by 8 bytes
  N   - divisible by 2
-------------------------------------------------------------------------*/
void vec_shift  (   f24 * restrict y, const    f24 * restrict x, int t, int N);
void vec_shift32(int32_t* restrict y, const int32_t* restrict x, int t, int N);
void vec_scale  (   f24 * restrict y, const    f24 * restrict x, f24 s, int N);

/*-------------------------------------------------------------------------
  This routine returns the fractional and exponential portion of the
  reciprocal of an vector x of Q31 numbers. Since the reciprocal is always
  greater than 1, it returns fractional portion frac in Q(31-exp) format
  and exponent exp so true reciprocal value in the Q0.31 may be found by
  shifting fractional part left by exponent value.
  For a reciprocal of 0, the result is not defined (but still be close to
  the maximum representative number)
  Accuracy is 10LSB (5*10-7) of fractional part

  Input:
  x[N]      - input data.
  N         - length of vectors
  Output:
  frac[N]   - fractional part of result
  exp[N]    - exponent part of result
  returned value:
  none

  Restriction:
  x,frac,exp should not be overlapping

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  frac,x   - should be aligned by 8 bytes
  N   - divisible by 2 and >2
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void vec_recip24 (
          f24   *restrict frac, // [N]
          short *restrict exp,  // [N]
    const f24   *restrict x,    // [N]
          int   N
);

// returns packed result, exp resides in 8 most significant bits
int32_t scl_recip24(f24 x);

/*-------------------------------------------------------------------------
  This routine performs pair wise division of vectors written in Q31 format.
  It returns the fractional and exponential portion of the division result.
  Since the division may generate result greater than 1, it returns
  fractional portion frac in Q(31-exp) format and exponent exp so true
  division result in the Q0.31 may be found by shifting fractional part
  left by exponent value.
  For division to 0, the result is not defined

  Accuracy is 10LSB (5*10-7) of fractional part

  Input:
  x[N]      - input data (dividend).
  y[N]      - input data (divisor).
  N         - length of vectors
  Output:
  frac[N]   - fractional part of result
  exp[N]    - exponent part of result
  returned value:
  none

  Restrictions:
  1. x,y,frac,exp should not be overlapping
  2. divisors should not be zero (otherwise result is not defined)


  PERFORMANCE NOTE:
  for optimum performance follow rules:
  frac,x,y   - should be aligned by 8 bytes
  N   - divisible by 2 and >2
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void vec_divide (
          f24   *restrict frac, // [N]
          short *restrict exp,  // [N]
    const f24   *restrict x,    // [N]
    const f24   *restrict y,    // [N]
    int N
);

// returns packed result, exp resides in 8 most significant bits
int32_t scl_divide(f24 x, f24 y);

/*-------------------------------------------------------------------------
  Base 2, natural and base 10 logarithm for vectorized data

  Input:
  x[N]      - input data, Q8
  N         - length of vectors
  Output:
  y[N]      - results, Q25

  Results are represented in Q25 format or 0x80000000 on
  negative of zero argument.
  This means, particulary, that
  log2(2^8)         -> 0
  log2(2^9)         -> 0x2000000*(9-8)
  log2(2^30)        -> 0x2000000*(30-8)

  Accuracy:
  vec_logx() 4.5e-5 worst case.
  scl_log2() 1.1e-5 worst case.
  scl_logn() 8e-6 worst case
  scl_log10() 4e-6 worst case

  Restrictions:
  none

  PERFORMANCE NOTES:
  1. for optimum performance follow rules:
     x   - should be aligned by 8 bytes
     N   - divisible by 2
     otherwise performance not guaranteed
  2. scl_logx() functions accepts true 32-bit arguments
     and provide better accuracy
-------------------------------------------------------------------------*/
void vec_log2 ( f24 * restrict y, const f24 * restrict x, int N );
void vec_logn ( f24 * restrict y, const f24 * restrict x, int N );
void vec_log10( f24 * restrict y, const f24 * restrict x, int N );

int32_t scl_log2 (int32_t x);
int32_t scl_logn (int32_t x);
int32_t scl_log10(int32_t x);

/*-------------------------------------------------------------------------
  Sine/cosine for vectorized data. Calculates sin(pi*x) or cos(pi*x)
  Input:
  x[N]      - input data, Q31 format
  N         - length of vectors
  Output:
  y[N]      - results, Q31 format

  Accuracy 6.1e-5 worst case.

  Restriction:
  x,y should not be overlapping

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,y - should be aligned by 8 bytes
  N   - divisible by 2
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void vec_sine  (f24 *restrict y, const f24 *restrict x, int N);
void vec_cosine(f24 *restrict y, const f24 *restrict x, int N);

f24 scl_sine  (f24 x);
f24 scl_cosine(f24 x);

/*-------------------------------------------------------------------------
  Tangent for vectorized data. Calculates tan(pi*x)
  Input:
  x[N]      - input data, Q31 format
  N         - length of vectors
  Output:
  y[N]      - fractional part of result, Q31 format
  exp[N]    - result exponent value

  Absolute accuracy 1.9e-5 for x=(-pi/4:pi/4),(-pi*5/4:pi*7/4)
  Relative accuracy  5.e-5 for other values

  Restriction:
  x,y should not be overlapping

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x   - should be aligned by 8 bytes
  N   - divisible by 2 and >4
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void vec_tan (f24 *restrict y, short *restrict exp,  const f24 *restrict x, int N);

// returns packed result, exp resides in 8 most significant bits
int32_t scl_tan(f24 x);

/*-------------------------------------------------------------------------
  Full arctangent for vectorized data. Calculates atan2(y,x)/pi
  Input:
  x[2*N]    - input data, Q31 format, complex data, real part goes first
  N         - length of vectors
  Output:
  z[N]      - results, Q31 format

  Absolute phase accuracy 1.8e-6
  if y==0 && x==0 returns 0

  Restriction:
  x,y,z should not be overlapping

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,y,z   - should be aligned by 8 bytes
  N       - divisible by 2 and >4
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void vec_atan2 (f24 *restrict y, const f24 *restrict x, int N);
f24 scl_atan2(f24 re, f24 im);

/*-------------------------------------------------------------------------
  Arctangent for vectorized data. Calculates atan(x)/pi for x in the range
  [-1,1) (Q31) and returns the result [-0.25,0.25] (Q31, bounds correspond
  to +pi/4).

  This function is faster than vec_atan2(), so it is preferable when the
  full phase is not required.

  Input:
  x[N] - input data
  N    - vector length
  Output:
  z[N] - results

  Absolute phase accuracy is not worse than 1.8e-6

  Restrictions:
  x,y should not overlap

  PERFORMANCE NOTES:
  For optimum performance obey the following rule:
  x,y - aligned on 8-byte boundary
  N - a multiple of 2
-------------------------------------------------------------------------*/
void vec_atan (f24 *restrict y, const f24 *restrict x, int N);
f24 scl_atan(f24 x) ;

/*-------------------------------------------------------------------------
  Exponent of all values in a vector

  This function performs a determination of the block
  exponent of the vector and returns the maximum exponent.
  This information can then be used to re-normalize the vector. Normally
  it is used in the FFT

  Input:
  x[N]      - input data.
  N         - length of vectors
  returned value:
  maximum exponent

  Restriction:
  none

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x   - should be aligned by 8 bytes
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
int vec_bexp   (const f24 * restrict x, int N);
int vec_bexp32 (const int32_t * restrict x, int N);
int scl_bexp   (f24 x);
int scl_bexp32 (int32_t x);

/*===========================================================================
    Matrix operations:
    mtx_mpy       Matrix multiply
    mtx_trans     Matrix transpose
===========================================================================*/

/*-------------------------------------------------------------------------
  Matrix multiply

  Input:
  x[N][M]   - input matrix NxP, Q31
  y[M][P]   - input matrix MxP, Q31
  N         - height of matrix x
  M         - width of matrix x and height of matrix y
  P         - width of matrix y

  Output:
  z[N][P]   - resulted matrix, Q30

  Restrictions:
  arrays should not be overlapping

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  x,y     - should be aligned by 8 bytes
  N,M,P   - divisible by 2 and >=4
  otherwise performance not guaranteed
-------------------------------------------------------------------------*/
void mtx_mpy (
          f24 * restrict z,// [N][P]
    const f24 *restrict x, // [N][M]
    const f24 *restrict y, // [M][P]
    int N, int M, int P
);

/*-------------------------------------------------------------------------
  Matrix transpose

  Input:
  x[N][M]   - input matrix
  N         - height of matrix
  M         - width of matrix
  Output:
  y[M][N]   - transposed matrix

  Restrictions:
  arrays should not be overlapping

  PERFORMANCE NOTES:
  1. for optimum performance follow rules:
     x,y   - should be aligned by 8 bytes
     N,M   - divisible by 2 and >=4
    otherwise performance not guaranteed
  2. Partial optimization is done for cases when N or M is even however
     nonaligned memory accesses will slow performance

-------------------------------------------------------------------------*/
void mtx_trans (
          f24 *restrict y,   // [M][N]
    const f24 *restrict x,   // [N][M]
    int N, int M
);


/*===========================================================================
  FFT section:
  fft_cplx   FFT on complex data
  fft_real   FFT on real data
  ifft_cplx  Inverse FFT on complex data
  ifft_real  Inverse FFT forming real data
===========================================================================*/

/*-------------------------------------------------------------------------
  FFT for complex data

  NOTES:
  1. Bit-reversal reordering is done here.
  2. FFT runs in-place so INPUT DATA WILL APPEAR DAMAGED after the call.

  Input:
  x[N*2]     - input signal. Real and imaginary data are interleaved with
               the real part going first for each number. The array is utilised
               for in-place calculations
  N          - FFT size
  scalingOpt - scaling option:
                0 – no scaling
                1 – 24-bit scaling
                2 – 32-bit scaling on the first stage and 24-bit scaling later
                3 – fixed scaling before each stage

  Output:
  y[N*2] - output spectrum

  Returned value: total number of left shifts occurred during scaling procedure

  Restrictions:
  Arrays should not overlap
  x,y - aligned on a 8-bytes boundary
  N   - 2^m: 16...4096

  PERFORMANCE NOTES:
  None
-------------------------------------------------------------------------*/
int fft_cplx(
              f24* y,
              int32_t* x,
              int N,
              int scalingOpt);
int fft_cplx_24x24(
              f24* y,
              int32_t* x,
              int N,
              int scalingOpt);


/*-------------------------------------------------------------------------
  Inverse FFT for complex data

  NOTES:
  1. Bit-reversal reordering is done here.
  2. IFFT runs in-place so INPUT DATA WILL APPEAR DAMAGED after the call.

  Input:
  x[N*2] - input spectrum. Real and imaginary data are interleaved with
           the real part going first for each number. The array is utilised
           for in-place calculations
  N      - IFFT size
  scalingOpt - scaling option:
                0 – no scaling
                1 – 24-bit scaling
                2 – 32-bit scaling on the first stage and 24-bit scaling later
                3 – fixed scaling before each stage
  Output:
  y[N*2] - output signal

  Returned value: total number of right shifts occurred during scaling procedure

  Restrictions:
  Arrays should not overlap
  x,y - aligned on a 8-bytes boundary
  N   - 2^m: 16...4096

  PERFORMANCE NOTES:
  None
-------------------------------------------------------------------------*/
int ifft_cplx(
              f24* y,
              int32_t* x,
              int N,
              int scalingOpt);
int ifft_cplx_24x24(
              f24* y,
              int32_t* x,
              int N,
              int scalingOpt);
/*-------------------------------------------------------------------------
  FFT on real data forming half of spectrum

  NOTES:
  1. Bit-reversal reordering is done here.
  2. FFT runs in-place so INPUT DATA WILL APPEAR DAMAGED after the call.
  3. Real data FFT function calls fft_cplx() to apply complex FFT of size
     N/2 to input data and then transforms the resulting spectrum.

  Input:
  x[N]         - input signal
  N            - FFT size
  scalingOpt   - scaling option:
                0 – no scaling
                1 – 24-bit scaling
                2 – 32-bit scaling on the first stage and 24-bit scaling later
                3 – fixed scaling before each stage

  Output:
  y[(N/2+1)*2] - output spectrum (positive side)

  Returned value: total number of right shifts occurred during scaling procedure

  Restrictions:
  Arrays should not overlap
  x,y - aligned on a 8-bytes boundary
  N   - 2^m: 32...8192

  PERFORMANCE NOTES:
  None
-------------------------------------------------------------------------*/
int fft_real(
              f24* y,
              int32_t* x,
              int N,
              int scalingOpt);
int fft_real_24x24(
              f24* y,
              int32_t* x,
              int N,
              int scalingOpt);
/*-------------------------------------------------------------------------
  Inverse FFT forming real data

  NOTES:
  1. Bit-reversing reordering is done here.
  2. IFFT runs in-place algorithm so INPUT DATA WILL APPEAR DAMAGED after call.
  3. Inverse FFT function for real signal transforms the input spectrum and
     then calls ifft_cplx() with FFT size set to N/2.

  Input:
  x[(N/2+1)*2]	input spectrum. Real and imaginary data are interleaved and
                real data goes first
   N		    FFT size
   scalingOpt	scaling option:
                0 – no scaling
                1 – 24-bit scaling
                2 – 32-bit scaling on the first stage and 24-bit scaling later
                3 – fixed scaling before each stage
   Output:
   y[N]	        real output signal

  Restrictions:
  Arrays should not overlap
  x,y - aligned on a 8-bytes boundary
  N   - 2^m: 32...8192

  PERFORMANCE NOTES:
  None
-------------------------------------------------------------------------*/
int ifft_real(
              f24* y,
              int32_t* x,
              int N,
              int scalingOpt);
int ifft_real_24x24(
              f24* y,
              int32_t* x,
              int N,
              int scalingOpt);
/* Returns library version string
   Parameters:
     version_string   Pre-allocated buffer for version string.
   Restrictions:
     version_string must points to a buffer large enough to hold up to
     30 characters.
 */
void NatureDSP_Signal_get_library_version(char *version_string);

/* Returns library API version string
   Parameters:
     version_string   Pre-allocated buffer for API version string.
   Restrictions:
     version_string must points to a buffer large enough to hold up to
     30 characters.
 */
void NatureDSP_Signal_get_library_api_version(char *version_string);

#ifdef __cplusplus
}
#endif

#endif//__NATUREDSP_SIGNAL_H__

